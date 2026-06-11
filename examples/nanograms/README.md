

# Procedure

## Download sample data
Download data of Run10 from below link
https://drive.google.com/drive/folders/1YndL2YL--97KLRYWP-ZWbz2emPajRk3L?usp=sharing

## Produce table files for calibration
'''
  ./calcCalFiles.py
'''

## Covert TPCTree into hittree
'''
  ./convertHitTree.rb
'''

## Plot signals for quick look (can be skipped)
'''
  cd quicklook
  ./plot_quicklook_events.py 
'''

## Event reconstruction
'''
  ./extract_compton_events.rb
'''
