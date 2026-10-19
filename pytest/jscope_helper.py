#/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
# * Use, distribution and modification of this code is permitted under the
# * terms stated in the Alif Semiconductor Software License Agreement
# *
# * You should have received a copy of the Alif Semiconductor Software
# * License Agreement with this file. If not, please write to:
# * contact@alifsemi.com, or visit: https://alifsemi.com/license
# *
# */

"""Joulescope (JS220 / JS320) helpers for per-phase power measurement of one inference round.

The DUT toggles one GPIO line (``GPIO_PROFILING``), which is wired to the Joulescope GPI input.
One round consists of five edges::

    rising   falling    rising     falling       rising
    pre start | inference | post start | base start | next round start
              |           |            |            |
    <- pre -->|<- inf --->|<- post --->|<-- base -->|

The phases are cut from a single, gap-free stream of current, voltage and GPI samples. The GPI
is sampled synchronously with current and voltage, so the phase boundaries have the resolution
of one sample (0.5 us at 2 MS/s) and do not depend on host side timing. Energy is the sum of
the sample-by-sample product ``i * v`` divided by the sample rate, so no sample is lost and no
sample is counted twice: the phases partition the round exactly.

Typical use::

    with RoundCapture() as capture:
        capture.start()                  # before the DUT is started / reset
        ...                              # start the test (reset DUT, etc.)
        result = capture.wait_for_round(timeout=120)
    print(result.summary())

The data analysis (:func:`analyze_round`) does not need any hardware or driver.
"""

import collections
import dataclasses
import threading
import time
from typing import Any, Callable, Deque, Dict, List, Optional, Tuple

import numpy as np

PHASE_NAMES = ('preprocessing', 'inference', 'postprocessing', 'base')
EDGES_PER_ROUND = len(PHASE_NAMES) + 1  # rising, falling, rising, falling, rising


@dataclasses.dataclass(frozen=True)
class SignalStats:
    """Basic statistics of one signal within one phase."""
    mean: float
    minimum: float
    maximum: float
    std: float


@dataclasses.dataclass(frozen=True)
class PhaseStats:
    """Measurement result of one phase of a round.

    Units: time in microseconds, current in mA, voltage in V, power in mW, energy in mJ.
    """
    time_us: float
    current_ma: SignalStats
    voltage_v: SignalStats
    power_mw: SignalStats
    energy_mj: float

    @property
    def time_ms(self) -> float:
        """Phase duration in milliseconds."""
        return self.time_us / 1000.0


@dataclasses.dataclass(frozen=True)
class RoundResult:
    """Measurement result of one complete round, including the base (idle) phase.

    ``edges`` are the sample offsets of the five GPIO edges from the first sample of the round.
    """
    sample_rate_hz: float
    phases: Dict[str, PhaseStats]
    edges: Tuple[int, ...]

    @property
    def total_time_ms(self) -> float:
        """Duration of the whole round (first rising edge to next rising edge) in ms."""
        return sum(p.time_ms for p in self.phases.values())

    @property
    def total_energy_mj(self) -> float:
        """Energy of the whole round in mJ."""
        return sum(p.energy_mj for p in self.phases.values())

    @property
    def mean_power_mw(self) -> float:
        """Average power over the whole round in mW."""
        return self.total_energy_mj / self.total_time_ms * 1000.0

    def summary(self) -> str:
        """Format the result as a table.

        :returns:   Multi-line string with one row per phase and a total row.
        """
        lines = ['phase           time [ms]   I [mA]    V [V]   P [mW]   E [mJ]']
        for name, phase in self.phases.items():
            lines.append(f'{name:<14} {phase.time_ms:10.4f} {phase.current_ma.mean:8.3f} '
                         f'{phase.voltage_v.mean:8.4f} {phase.power_mw.mean:8.3f} '
                         f'{phase.energy_mj:8.5f}')
        lines.append(f'{"round":<14} {self.total_time_ms:10.4f} {"":8} {"":8} '
                     f'{self.mean_power_mw:8.3f} {self.total_energy_mj:8.5f}')
        return '\n'.join(lines)


def find_round_edges(gpi: np.ndarray) -> Optional[np.ndarray]:
    """Find the first complete round in a GPI sample array.

    :param gpi: GPI samples (0 / 1).
    :returns:   Sample indices of the five edges (rising, falling, rising, falling, rising),
                where each index is the first sample after the edge, or None if the array
                does not contain a complete round.
    """
    level = np.asarray(gpi).astype(np.int8)
    edges = np.flatnonzero(np.diff(level)) + 1
    rising = np.flatnonzero(level[edges] == 1)
    if rising.size == 0 or rising[0] + EDGES_PER_ROUND > edges.size:
        return None
    return edges[rising[0]:rising[0] + EDGES_PER_ROUND]


def _signal_stats(values: np.ndarray, mean: float) -> SignalStats:
    """Statistics of a sample array, with an externally computed (time weighted) mean."""
    return SignalStats(mean=mean, minimum=float(np.min(values)), maximum=float(np.max(values)),
                       std=float(np.std(values)))


def _scaled(stats: SignalStats, scale: float) -> SignalStats:
    return SignalStats(*(v * scale for v in dataclasses.astuple(stats)))


def compute_phase_stats(current: np.ndarray, voltage: np.ndarray, id_rate_hz: float,
                        edges: np.ndarray, first_id: int = 0,
                        ids_per_sample: int = 1) -> Dict[str, PhaseStats]:
    """Compute per-phase statistics between consecutive edges.

    The edges are positions on the id timeline (``id_rate_hz`` ids per second). Sample ``k`` of
    current and voltage is valid from id ``first_id + k * ids_per_sample`` for ids_per_sample
    ids, which is how the Joulescope driver numbers decimated samples. An edge that falls
    inside a sample splits that sample proportionally, so phase energies add up exactly to the
    energy of the round. Means are time weighted, minimum, maximum and std use the samples whose
    middle is inside the phase.

    :param current:         Current samples in A.
    :param voltage:         Voltage samples in V, same length as current.
    :param id_rate_hz:      Id rate in Hz (sample rate before decimation).
    :param edges:           The five round edges as ids, see :func:`find_round_edges`.
    :param first_id:        Id of the first current / voltage sample.
    :param ids_per_sample:  Decimation of current / voltage, ids per sample.
    :returns:               Statistics for each phase, keyed by PHASE_NAMES.
    """
    current = np.asarray(current, dtype=np.float64)
    voltage = np.asarray(voltage, dtype=np.float64)
    power = current * voltage  # sample-by-sample, not mean(i) * mean(v)
    grid = np.arange(current.size + 1)
    # Cumulative integrals (in samples) of piecewise constant signals, exact at any position
    integrals = {name: np.concatenate(([0.0], np.cumsum(values)))
                 for name, values in (('i', current), ('v', voltage), ('p', power))}

    def area(name: str, start: float, stop: float) -> float:
        return float(np.interp((stop - first_id) / ids_per_sample, grid, integrals[name])
                     - np.interp((start - first_id) / ids_per_sample, grid, integrals[name]))

    phases = {}
    for name, start, stop in zip(PHASE_NAMES, edges[:-1], edges[1:]):
        samples = (stop - start) / ids_per_sample
        lo = int(np.ceil((start - first_id) / ids_per_sample - 0.5))
        hi = max(int(np.ceil((stop - first_id) / ids_per_sample - 0.5)), lo + 1)
        lo, hi = min(max(lo, 0), current.size - 1), min(max(hi, 1), current.size)
        hi = max(hi, lo + 1)
        energy_j = area('p', start, stop) * ids_per_sample / id_rate_hz
        phases[name] = PhaseStats(
            time_us=(stop - start) / id_rate_hz * 1e6,
            current_ma=_scaled(_signal_stats(current[lo:hi], area('i', start, stop) / samples),
                               1e3),
            voltage_v=_signal_stats(voltage[lo:hi], area('v', start, stop) / samples),
            power_mw=_scaled(_signal_stats(power[lo:hi], area('p', start, stop) / samples), 1e3),
            energy_mj=energy_j * 1e3)
    return phases


def analyze_round(current: np.ndarray, voltage: np.ndarray, gpi: np.ndarray,
                  sample_rate_hz: float) -> RoundResult:
    """Split the first complete round of a capture into phases and compute the statistics.

    :param current:         Current samples in A.
    :param voltage:         Voltage samples in V, same length as current.
    :param gpi:             GPI samples (0 / 1), same length as current.
    :param sample_rate_hz:  Sample rate in Hz.
    :returns:               The measurement result for the round.
    :raises ValueError:     If the capture does not contain a complete round.
    """
    edges = find_round_edges(gpi)
    if edges is None:
        raise ValueError('No complete round (5 GPIO edges, starting rising) found in capture')
    return RoundResult(sample_rate_hz=sample_rate_hz,
                       phases=compute_phase_stats(current, voltage, sample_rate_hz, edges),
                       edges=tuple(int(e - edges[0]) for e in edges))


class _SampleStream:
    """Contiguous sample accumulator of one signal.

    Positions are driver sample ids, which count at the rate before decimation. A stream with
    ``decimate`` d has one sample per d ids.
    """

    def __init__(self):
        self._chunks: Deque[Tuple[int, np.ndarray]] = collections.deque()
        self.decimate = 1
        self.origin: Optional[int] = None  # id of the first sample since the last clear()
        self.next_id: Optional[int] = None  # id after the last sample

    def clear(self):
        """Drop all samples and forget the position."""
        self._chunks.clear()
        self.origin = None
        self.next_id = None

    def add(self, start: int, decimate: int, data: np.ndarray) -> bool:
        """Append a chunk.

        :param start:       Id of the first sample of the chunk.
        :param decimate:    Ids per sample.
        :param data:        The samples.
        :returns:           False if samples are missing between the previous chunk and this
                            one, or if the decimation changed. The chunk is not stored then.
        """
        if self.next_id is not None:
            if decimate != self.decimate or start > self.next_id:
                return False
            skip = (self.next_id - start) // decimate  # overlap with the previous chunk
            data = data[skip:]
            start = self.next_id
        else:
            self.origin = start
            self.decimate = decimate
        if data.size:
            self._chunks.append((start, data))
            self.next_id = start + data.size * decimate
        return True

    def prune(self, keep_ids: int):
        """Drop whole chunks that end more than keep_ids before the newest sample.

        :param keep_ids:    Number of newest ids to keep at least.
        """
        if self.next_id is None:
            return
        floor = self.next_id - keep_ids
        while (len(self._chunks) > 1
               and self._chunks[0][0] + self._chunks[0][1].size * self.decimate <= floor):
            self._chunks.popleft()

    def get(self, start: int, stop: int) -> Tuple[np.ndarray, int]:
        """Get the samples covering the ids [start, stop).

        :param start:       First id.
        :param stop:        Id after the last id.
        :returns:           The samples and the id of the first returned sample, which
                            is at or before start.
        :raises ValueError: If the range is not available.
        """
        if not self._chunks:
            raise ValueError('No samples')
        base = self._chunks[0][0]
        data = np.concatenate([chunk for _, chunk in self._chunks])
        first = (start - base) // self.decimate
        last = -((base - stop) // self.decimate)  # ceil
        if first < 0 or last > data.size:
            raise ValueError(f'Samples {start}:{stop} not available')
        return data[first:last], base + first * self.decimate


class _GpiTrigger:
    """Finds the five edges of the first complete round in a stream of GPI samples."""

    def __init__(self):
        self.edges: List[int] = []
        self._level: Optional[int] = None
        self._next_id: Optional[int] = None

    @property
    def done(self) -> bool:
        """True when a complete round has been found."""
        return len(self.edges) >= EDGES_PER_ROUND

    def reset_continuity(self):
        """Forget the previous level, used after samples were lost before the first edge."""
        self._level = None
        self._next_id = None

    def add(self, start: int, decimate: int, bits: np.ndarray, min_id: Optional[int]) -> bool:
        """Process a chunk of unpacked GPI samples.

        :param start:       Id of the first sample of the chunk.
        :param decimate:    Ids per sample.
        :param bits:        GPI samples (0 / 1).
        :param min_id:      A round must start at or after this id (the first id for which
                            current and voltage are available). None: no round can start yet.
        :returns:           False if samples are missing before this chunk.
        """
        if self.done:
            return True
        if self._next_id is not None:
            if start > self._next_id:
                return False
            bits = bits[(self._next_id - start) // decimate:]
            start = self._next_id
        if bits.size == 0:
            return True
        levels = bits.astype(np.int8)
        if self._level is None:
            extended, offset = levels, 1  # the first sample has no predecessor
        else:
            extended, offset = np.concatenate((np.array([self._level], np.int8), levels)), 0
        for index in np.flatnonzero(np.diff(extended)) + offset:
            edge_id = start + int(index) * decimate
            if not self.edges and (levels[index] != 1 or min_id is None or edge_id < min_id):
                continue  # a round always starts with a rising edge that can be measured
            self.edges.append(edge_id)
            if self.done:
                break
        self._level = int(levels[-1])
        self._next_id = start + levels.size * decimate
        return True


class _RoundRecorder:
    """Receives the driver data callbacks and collects the first complete round.

    The callbacks run on the driver thread, everything is protected by a lock. Before the first
    rising edge only the newest ``pre_roll_s`` of current and voltage samples are kept, so a
    capture started long before the test can run without growing memory.
    """

    def __init__(self, pre_roll_s: float):
        self._pre_roll_s = pre_roll_s
        self._lock = threading.Lock()
        self._streams = {'current': _SampleStream(), 'voltage': _SampleStream()}
        self._trigger = _GpiTrigger()
        self._done = threading.Event()
        self._error: Optional[str] = None
        self._rate = 0.0

    @staticmethod
    def _parse(value: Dict[str, Any]) -> Tuple[int, int, float, np.ndarray]:
        """Extract (first id, ids per sample, id rate, copy of data) from a driver message."""
        return (int(value['sample_id']), int(value.get('decimate_factor') or 1),
                float(value['sample_rate']), np.array(value['data'], copy=True))

    def _fail(self, message: str):
        """Abort the capture (lock must be held)."""
        self._error = self._error or message
        self._done.set()

    def _min_round_id(self) -> Optional[int]:
        """First id at which both current and voltage are available (lock must be held)."""
        origins = [s.origin for s in self._streams.values()]
        return None if None in origins else max(origins)

    def on_samples(self, name: str, value: Dict[str, Any]):
        """Handle a current or voltage chunk.

        :param name:    'current' or 'voltage'.
        :param value:   The driver data message.
        """
        try:
            start, decimate, rate, data = self._parse(value)
            with self._lock:
                self._rate = rate
                stream = self._streams[name]
                if not stream.add(start, decimate, data):
                    edges = self._trigger.edges
                    if edges and start > edges[0]:
                        self._fail(f'Samples lost in {name} stream during the measured round')
                        return
                    stream.clear()  # gap before the round, start over
                    stream.add(start, decimate, data)
                if not self._trigger.edges:
                    stream.prune(int(self._pre_roll_s * rate))
        except (KeyError, TypeError, ValueError) as exc:
            with self._lock:
                self._fail(f'Bad {name} data: {exc!r}')

    def on_gpi(self, value: Dict[str, Any]):
        """Handle a GPI chunk (bit packed, least significant bit first).

        :param value:   The driver data message.
        """
        try:
            start, decimate, rate, packed = self._parse(value)
            bits = np.unpackbits(packed.astype(np.uint8), bitorder='little')
            with self._lock:
                self._rate = rate
                min_id = self._min_round_id()
                if not self._trigger.add(start, decimate, bits, min_id):
                    if self._trigger.edges:
                        self._fail('Samples lost in GPI stream during the measured round')
                        return
                    self._trigger.reset_continuity()
                    self._trigger.add(start, decimate, bits, min_id)
                if self._trigger.done:
                    self._done.set()
        except (KeyError, TypeError, ValueError) as exc:
            with self._lock:
                self._fail(f'Bad GPI data: {exc!r}')

    def _result(self) -> Optional[RoundResult]:
        """Analyze the round if current and voltage cover it (lock must be held)."""
        edges = np.array(self._trigger.edges)
        current, voltage = self._streams['current'], self._streams['voltage']
        if any(s.next_id is None or s.next_id < edges[-1] + s.decimate
               for s in (current, voltage)):
            return None
        try:
            i_data, i_first = current.get(edges[0], edges[-1])
            v_data, v_first = voltage.get(edges[0], edges[-1])
        except ValueError as exc:
            raise RuntimeError(str(exc)) from exc
        if i_first != v_first or i_data.size != v_data.size or current.decimate != voltage.decimate:
            raise RuntimeError('Current and voltage samples are not aligned')
        return RoundResult(
            sample_rate_hz=self._rate,
            phases=compute_phase_stats(i_data, v_data, self._rate, edges - edges[0],
                                       i_first - edges[0], current.decimate),
            edges=tuple(int(e - edges[0]) for e in edges))

    def collect(self, timeout: float, settle_timeout: float = 2.0) -> RoundResult:
        """Wait for a complete round and analyze it.

        :param timeout:         Seconds to wait for the five edges.
        :param settle_timeout:  Seconds to wait for current and voltage to reach the last edge.
        :returns:               The measurement result.
        :raises TimeoutError:   If the round does not complete in time.
        :raises RuntimeError:   If samples were lost or the data was invalid.
        """
        if not self._done.wait(timeout):
            raise TimeoutError(f'No complete round (5 GPIO edges) within {timeout} s')
        deadline = time.monotonic() + settle_timeout
        while True:
            with self._lock:
                if self._error:
                    raise RuntimeError(self._error)
                result = self._result()
            if result is not None:
                return result
            if time.monotonic() > deadline:
                raise TimeoutError('Current / voltage samples did not reach the end of the round')
            time.sleep(0.001)


@dataclasses.dataclass
class CaptureConfig:
    """Configuration of a :class:`RoundCapture`.

    ``sample_rate_hz`` None selects 2 MHz, falling back to 1 MHz if the device does not accept
    it. ``settings`` are extra driver topics published after the device is opened, for example
    ``{'s/i/range/mode': 'manual', 's/i/range/select': '10 mA'}`` to avoid auto ranging.
    """
    serial_number: Optional[str] = None
    sample_rate_hz: Optional[int] = None
    gpi_index: int = 0
    pre_roll_s: float = 0.5
    settings: Dict[str, Any] = dataclasses.field(default_factory=dict)


class _DriverBus:
    """Publish / subscribe on one device of a ``pyjoulescope_driver.Driver``.

    Offers the same relative-topic interface as a ``joulescope`` Device.
    """

    def __init__(self, driver: Any, path: str):
        self._driver = driver
        self._path = path

    def publish(self, topic: str, value: Any):
        """Publish a value to a device topic."""
        self._driver.publish(f'{self._path}/{topic}', value)

    def subscribe(self, topic: str, flags: Any, fn: Any):
        """Subscribe to a device topic."""
        self._driver.subscribe(f'{self._path}/{topic}', flags, fn)

    def unsubscribe(self, topic: str, fn: Any):
        """Unsubscribe from a device topic."""
        self._driver.unsubscribe(f'{self._path}/{topic}', fn)


class RoundCapture:
    """Captures current, voltage and GPI from a JS220 / JS320 until one round is complete.

    :param config:  Capture configuration, None for defaults.
    :param source:  Where to get the Joulescope from. Either a ``joulescope`` Device (for
                    example from ``joulescope.scan()``, as in the pytest ``JScope`` fixture),
                    which is opened if it is not open yet, or a
                    ``pyjoulescope_driver.Driver``, or None to create a Driver.
    """

    def __init__(self, config: Optional[CaptureConfig] = None, source: Optional[Any] = None):
        self._config = config or CaptureConfig()
        self._source = source
        self._bus: Optional[Any] = None
        self._close_fn: Optional[Callable[[], None]] = None
        self._recorder: Optional[_RoundRecorder] = None
        self._callbacks: Optional[Dict[str, Any]] = None

    def __enter__(self) -> 'RoundCapture':
        self.open()
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()

    def _gpi_topic(self) -> str:
        return f's/gpi/{self._config.gpi_index}/'

    def _set_sample_rate(self, bus: Any, rates: List[int]):
        """Set the first sample rate in rates that the device accepts."""
        for index, rate in enumerate(rates):
            try:
                bus.publish('h/fs', rate)
                return
            except (ValueError, RuntimeError):
                if index == len(rates) - 1:
                    raise

    def _open_driver(self, driver: Any, finalize: bool) -> Callable[[], None]:
        """Open the selected device of a driver, returns the function that undoes it."""
        paths = driver.device_paths()
        serial = self._config.serial_number
        if serial:
            paths = [p for p in paths if p.lower().endswith(serial.lower())]
        if len(paths) != 1:
            raise RuntimeError(f'Expected exactly one Joulescope, found {paths}. '
                               'Set CaptureConfig.serial_number to select one.')
        path = paths[0]
        driver.open(path, mode='defaults')
        self._bus = _DriverBus(driver, path)
        rate = self._config.sample_rate_hz
        self._set_sample_rate(self._bus, [rate] if rate else [2000000, 1000000])

        def close():
            driver.close(path)
            if finalize:
                driver.finalize()
        return close

    def open(self):
        """Open the Joulescope and apply the configuration.

        :raises RuntimeError:   If a driver is used and not exactly one matching Joulescope is
                                found.
        """
        source = self._source
        if source is None:
            from pyjoulescope_driver import Driver  # pylint: disable=import-outside-toplevel
            self._close_fn = self._open_driver(Driver(), finalize=True)
        elif hasattr(source, 'device_paths'):
            self._close_fn = self._open_driver(source, finalize=False)
        else:  # joulescope Device, which already knows its path and the device defaults
            self._bus = source
            if not source.is_open:
                source.open()
                self._close_fn = source.close
            if self._config.sample_rate_hz:
                self._set_sample_rate(source, [self._config.sample_rate_hz])
        for topic, value in self._config.settings.items():
            self._bus.publish(topic, value)

    def close(self):
        """Stop streaming and close the Joulescope, if it was opened by this object."""
        self.stop()
        close_fn, self._close_fn = self._close_fn, None
        if close_fn is not None:
            close_fn()
        self._bus = None

    def _data_callbacks(self) -> Dict[str, Any]:
        recorder = self._recorder
        return {
            's/i/': lambda topic, value: recorder.on_samples('current', value),
            's/v/': lambda topic, value: recorder.on_samples('voltage', value),
            self._gpi_topic(): lambda topic, value: recorder.on_gpi(value),
        }

    def start(self):
        """Start streaming. Call before the test is started, so that nothing is missed."""
        self.stop()
        self._recorder = _RoundRecorder(self._config.pre_roll_s)
        self._callbacks = self._data_callbacks()
        for topic, fn in self._callbacks.items():
            self._bus.subscribe(topic + '!data', ['pub'], fn)
        for topic in self._callbacks:
            self._bus.publish(topic + 'ctrl', 1)

    def stop(self):
        """Stop streaming. Safe to call at any time."""
        callbacks, self._callbacks = self._callbacks, None
        if not callbacks:
            return
        for topic, fn in callbacks.items():
            self._bus.unsubscribe(topic + '!data', fn)
            self._bus.publish(topic + 'ctrl', 0)

    def wait_for_round(self, timeout: float) -> RoundResult:
        """Wait for the first complete round (five GPIO edges), stop streaming and analyze it.

        The first rising edge seen after :meth:`start` begins the round, so rounds skipped by
        the DUT (line kept low) are ignored. The capture ends at the next rising edge, which
        includes the whole base phase.

        :param timeout:         Seconds to wait for the round to complete.
        :returns:               The measurement result.
        :raises TimeoutError:   If the round does not complete in time.
        :raises RuntimeError:   If samples were lost during the round.
        """
        if self._recorder is None:
            raise RuntimeError('start() must be called before wait_for_round()')
        try:
            return self._recorder.collect(timeout)
        finally:
            self.stop()
