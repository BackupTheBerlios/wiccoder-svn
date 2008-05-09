# Imports
import pylab
import os
import re
import string
import pylab
import copy

# Wic imports
import pywic


# Constants
#use_wic_tool	= pywic.wic_tool_path
#use_wic_tool	= pywic.wic_tool_path_r
images_dir		= "../res/images/"
working_dir		= "./tmp/"

use_image		= "lenaD.bmp"
use_method		= "manual"
use_filter		= "Petuhov1"
use_bpp			= 0.5
use_q			= 13.2
use_lambda		= 21

# Get
def get_acoder_stats(use_wic_tool):
	try:
		os.makedirs(working_dir)
	except:
		pass

	src_image		= images_dir + use_image;
	wic_file		= string.join([working_dir + use_image, "wic"], ".")
	dest_image		= string.join([working_dir + use_image, use_wic_tool, "bmp"], ".")
	stats_caption 	= use_image + "; " + use_filter + "; bpp: " + str(use_bpp) + \
					  " (q=" + str(use_q) + ", l=" + str(use_lambda) + ")"
	stats_save_path = string.join([working_dir + use_image, "stats", use_wic_tool, "png"], ".")
	r = pywic.encode_image(src_image, wic_file, filter=use_filter, method=use_method,
						   q=use_q, l=use_lambda, bpp=use_bpp, wic_tool_path=use_wic_tool)
	pywic.plot_acoder_stats(r["acs_init"], r["acs_encd"], caption=stats_caption, save_path=stats_save_path)

	return r


# Compares debug and release stats
def compare_debug_and_release_stats():
	r_r = get_acoder_stats(pywic.wic_tool_path)
	r_d = get_acoder_stats(pywic.wic_tool_path_r)

	acs_init_r = r_r["acs_init"]
	acs_encd_r = r_r["acs_encd"]
	acs_init_d = r_d["acs_init"]
	acs_encd_d = r_d["acs_encd"]

	if acs_init_r != acs_init_d:
		print "acs_init different"
	if acs_encd_r != acs_encd_d:
		print "acs_encd different"

	#diff_i = {}
	#models_range_init = list(set(acs_init_r.keys() + acs_init_d.keys()))
	#for i in models_range_init:
	#	if acs_init_r[i].size != acs_init_d[i].size:
	#		print "[i]different size for", i,  acs_init_r[i].size, "!=", acs_init_d[i].size
	#
	#	diff_i[i] = copy.deepcopy(acs_init_r[i])
	#	diff_i[i].sub(acs_init_d[i])
	#
	#	stats_r = acs_init_r[i].stats
	#	stats_d = acs_init_d[i].stats
	#
	#	for j in range(0, min(len(stats_r), len(stats_d))):
	#		if stats_r[j] != stats_d[j]:
	#			print "[i] different value", i, "-", j, "->", stats_r[j], "!=", stats_d[j]
	#
	#diff_e = {}
	#models_range_encd = list(set(acs_encd_r.keys() + acs_encd_d.keys()))
	#for i in models_range_encd:
	#	if acs_encd_r[i].size != acs_encd_d[i].size:
	#		print "[e]different size for", i,  acs_encd_r[i].size, "!=", acs_encd_d[i].size
	#
	#	diff_e[i] = copy.deepcopy(acs_encd_r[i])
	#	diff_e[i].sub(acs_encd_d[i])
	#
	#	stats_r = acs_encd_r[i].stats
	#	stats_d = acs_encd_d[i].stats
	#
	#	for j in range(0, min(len(stats_r), len(stats_d))):
	#		if stats_r[j] != stats_d[j]:
	#			print "[e] different value", i, "-", j, "->", stats_r[j], "!=", stats_d[j]
	#
	#stats_save_path = string.join([working_dir + use_image, "stats", "diffs", "png"], ".")
	#pywic.plot_acoder_stats(diff_i, diff_e)#, save_path=stats_save_path)
	#stats_save_path = string.join([working_dir + use_image, "stats", "diffs", "png"], ".")
	#pywic.plot_acoder_stats(diff_i, diff_e)#, save_path=stats_save_path)


# Main function
def main():
	get_acoder_stats(pywic.wic_tool_path_r)
	#compare_debug_and_release_stats()

main()
