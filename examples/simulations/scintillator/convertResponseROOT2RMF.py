#!/usr/bin/env python
# version 2016-01-28
# Teruaki Enoto & Hirokazu Odaka

import os 
import sys 
import yaml
import numpy
import pyfits
import ROOT

def convert_ROOT_to_RMF(setup_yamlfile):

	if not os.path.exists(setupfile):
		sys.stderr.write('response setup file (%s) does not exist.\n' % setupfile)
		quit()		
	setup = yaml.load(open(setupfile))
	input_rootfile = setup['input_rootfile']
	output_rmffile = setup['output_rmffile']
	response_histname = setup['response_histname']
	number_of_channel = int(setup['number_of_channel'])
	energy_min  = float(setup['energy_min'])
	energy_step = float(setup['energy_step'])

	sys.stdout.write('\n==== convert_ROOT_to_RMF ====\n')
	sys.stdout.write('%s ---> %s\n' % (input_rootfile, output_rmffile))
	sys.stdout.write('from %.2f keV with %.2f keV step (channel = %d)\n' % 
		(energy_min, energy_step, number_of_channel))

	if not os.path.exists(input_rootfile):
		sys.stderr.write('%s does not exist.\n' % input_rootfile)
		quit()

	if os.path.exists(output_rmffile):
		sys.stderr.write('%s has already existed.\n' % output_rmffile)
		quit()

	f = ROOT.TFile(input_rootfile)
	#f.ls()
	hist = f.Get(response_histname)

	geometrical_area = float(setup['geometrical_area'])
  	number_of_photon = int(setup['number_of_photon'])
  	simulation_energy_min = float(setup['simulation_energy_min'])
  	simulation_energy_max = float(setup['simulation_energy_max'])
  	num_bins_in_simulation = (simulation_energy_max-simulation_energy_min)/energy_step 

	num_per_bin = number_of_photon/num_bins_in_simulation
	normalization = float(setup['normalization'])
  	hist.Scale(1./num_per_bin*geometrical_area*normalization)

  	matrix_form = "%sE" % number_of_channel
	n_grp  = 1
	f_chan = 1
	
	sys.stdout.write('filling the matrix. this procedure requires time.\n')
	energy_lo = numpy.array([hist.GetXaxis().GetBinLowEdge(i+1) for i in range(0,number_of_channel)])
	energy_hi = numpy.array([hist.GetXaxis().GetBinUpEdge(i+1) for i in range(0,number_of_channel)])	
	maxchan   = numpy.array([number_of_channel for i in range(0,number_of_channel)])
	matrix    = numpy.array([numpy.array([hist.GetBinContent(i+1,j+1) for j in range(0,number_of_channel)]) for i in range(0,number_of_channel)])
	col_matrix    = pyfits.Column(name='MATRIX',format=matrix_form, array=matrix)
	col_energy_lo = pyfits.Column(name='ENERG_LO',format="1E", array=energy_lo,unit="keV")
	col_energy_hi = pyfits.Column(name='ENERG_HI',format="1E", array=energy_hi,unit="keV")
	col_n_grp     = pyfits.Column(name='N_GRP',format="1I", array=numpy.array([n_grp for i in range(0,number_of_channel)]),unit="")
	col_f_chan    = pyfits.Column(name='F_CHAN',format="1I", array=numpy.array([f_chan for i in range(0,number_of_channel)]),unit="")
	col_n_chan    = pyfits.Column(name='N_CHAN',format="1I", array=maxchan,unit="")
	hdu1 = pyfits.ColDefs([col_energy_lo,col_energy_hi,col_n_grp,col_f_chan,col_n_chan,col_matrix])

	channel     = numpy.array([i for i in range(0,number_of_channel)])
	e_min       = numpy.array([energy_min+float(i)*energy_step for i in range(0,number_of_channel)])
	e_max       = numpy.array([energy_min+(float(i)+1)*energy_step for i in range(0,number_of_channel)])
	col_channel = pyfits.Column(name='CHANNEL',format="1I", array=channel, unit="chan")
	col_e_min   = pyfits.Column(name='E_MIN',format="1E", array=e_min, unit="keV")
	col_e_max   = pyfits.Column(name='E_MAX',format="1E", array=e_max, unit="keV")
	hdu2 = pyfits.ColDefs([col_channel,col_e_min,col_e_max])

	prhdu   = pyfits.PrimaryHDU()
	tbhdu1  = pyfits.new_table(hdu1)
	tbhdu2  = pyfits.new_table(hdu2)
	tbhdu1.name = 'MATRIX'
	tbhdu2.name = 'EBOUNDS'
	hdu     = pyfits.HDUList([prhdu, tbhdu1, tbhdu2])

	for i in range(0,3):
	  	for keyword,usrsetup in setup['keywords_common'].items():
			hdu[i].header[keyword] = usrsetup[0]
			hdu[i].header.comments[keyword] = usrsetup[1]
		hdu[i].header["FILENAME"] = output_rmffile
		hdu[i].header.comments[keyword] = "Filename"				
	i = 1
	hdu[i].header["DETCHANS"] = number_of_channel
	hdu[i].header.comments[keyword] = "total number of detector channels"

	hdu.writeto(output_rmffile)    
	sys.stdout.write('Creating FITS File is succeeded\n')


if __name__ == "__main__":
	if len(sys.argv) != 2:
		sys.stderr.write('%s response_setup.yaml \n' % sys.argv[0])
		quit()
	setupfile = sys.argv[1]
	convert_ROOT_to_RMF(setupfile)


	