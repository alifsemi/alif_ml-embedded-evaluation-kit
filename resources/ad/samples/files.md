# Sample wav audio clip

For this use case sample audio clips aren't provided. However, the file
`random_id_00_000000.wav` is provided as a dummy placeholder.

The data used for this application sample comes from
[https://zenodo.org/record/3384388\#.X6GILFNKiqA](https://zenodo.org/record/3384388\#.X6GILFNKiqA)
and the model included in this example is trained on the ‘Slider’ part of the dataset.

The machine ID (00, 02, 04, 06) the clip comes from must be in the file name for the application to work.

The file name should have a pattern that matches
e.g. `<any>_<text>_00_<here>.wav` if the audio was from machine ID 00
or `<any>_<text>_02_<here>.wav` if it was from machine ID 02 etc.
For example:

- `anomaly_id_00_00000000.wav`
- `normal_id_00_00000004.wav`
