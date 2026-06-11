# How to use this example

## Download sample data
Download data of Run10 from below link

https://drive.google.com/drive/folders/1EK_Q2g5pwMILerbwJzrgySzq8NcVql4G?usp=sharing

Move the test data to the proper directory

```
  mv /path/to/data /path/to/ComptonSoft/example/nanograms
  cd /path/to/ComptonSoft/example/nanograms
```

## Produce table files for calibration
```
  ./calcCalFiles.py
```

## Covert TPCTree into hittree
```
  ./convertHitTree.rb
```

## Plot signals for quick look (can be skipped)
```
  cd quicklook
  ./plot_quicklook_events.py 
```

## Event reconstruction
```
  ./extract_compton_events.rb
```
