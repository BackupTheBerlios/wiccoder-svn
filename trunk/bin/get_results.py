# Imports
import os
import re
import string

# Wic imports
from pywic import *

# Constants
images_dir		= "../res/images/"
working_dir		= "./tmp/"

images			= ["lenaD.bmp", "barbaraD.bmp", "goldhillD.bmp"]
#images			= ["lenaR.bmp", "barbaraR.bmp", "goldhillR.bmp"]
#images			= ["lenaR.bmp"]
# filters			= ["Antonini", "cdf97", "Petuhov1"]
#filters			= ["Antonini", "Petuhov1"]
filters			= ["Antonini"]
bpps			= [0.25, 0.5, 1.0]
#bpps			= [0.2568, 0.3557, 0.5629, 0.7876, 0.9875, 1.322]	# lena jpeg2k
#bpps			= [0.3247, 0.39, 0.4876, 0.6578, 0.787, 0.9908, 1.165, 1.325] # barbara jpeg2k
#bpps			= [0.2414, 0.3246, 0.3896, 0.4913, 0.6012, 0.7881, 0.9917, 1.321] # goldhill jpeg2k
#bpps			= [0.50]
#bpps			= [0.20, 0.25, 0.30, 0.35, 0.40, 0.45, 0.50, 0.55, 0.60, 0.65, 0.70, 0.75, 0.80, 0.85, 0.90, 0.95, 1.00, 1.05, 1.10, 1.15, 1.20, 1.25, 1.30, 1.35, 1.40, 1.45, 1.50]


# Main function
def main():
	try:
		os.makedirs(working_dir)
	except:
		pass

	avg_psnr	= 0
	n_psnrs		= 0

	for filter in filters:
		# Print table head
		print filter + ":"
		print "-"*79
		fw = 18
		table_head = "|" + "bpp".center(fw)
		for image in images:
			table_head += "|" + image.center(fw)
		print table_head

		for bpp in bpps:
			print "-"*79
			table_line = "|" + str(bpp).center(fw)
			for image in images:
				src_image	= images_dir + image;
				wic_file	= string.join([working_dir + image, filter, str(bpp), "wic"], ".")
				dest_image	= string.join([working_dir + image, filter, str(bpp), "bmp"], ".")
				r = encode_image(src_image, wic_file, filter=filter, method="fixed_bpp", bpp=bpp)
				decode_image(wic_file, dest_image)
				psnr = calc_psnr(src_image, dest_image)

				do_plot_acoder_stats = False;

				# Plot acoder stats
				if do_plot_acoder_stats:
					stats_caption 	= image + "; " + filter + "; bpp: " + str(bpp) + "; q=" + str(r["q"]) + "; l=" + str(r["l"]) + "; psnr=" + str(psnr)
					wic_stats_file 	= string.join([working_dir + image, filter, str(bpp), "stats", "wic"], ".")
					dest_stats_image= string.join([working_dir + image, filter, str(bpp), "stats", "bmp"], ".")
					stats_save_path = string.join([working_dir + image, filter, str(bpp), "stats", "png"], ".")
					stats_save_patt = string.join([working_dir + image, filter, str(bpp), "stats", "txt"], ".")

					stats_r = encode_image(src_image, wic_stats_file, filter=filter, method="manual", q=r["q"], l=r["l"], wic_tool_path=wic_tool_path_r)
					decode_image(wic_stats_file, dest_stats_image, wic_tool_path=wic_tool_path_r)
					write_acoder_stats(stats_r["acs_init"], stats_r["acs_encd"],
									   about=stats_caption, save_path=stats_save_patt);
					plot_acoder_stats(stats_r["acs_init"], stats_r["acs_encd"], caption=stats_caption,
									  save_path=stats_save_path, models_range=range(1, 6))

				# table_line += "|" + (str(psnr) + "/" + str(round(r["bpp"], 2)) + "/" + str(stats_psnr)).center(fw)
				table_line += "|" + (str(psnr) + " / " + str(round(r["bpp"], 2))).center(fw)

				# Don't want psnr's with bad bpp
				if 0.01 < abs(r["bpp"] - bpp): continue

				# Calculate average psnr
				n_psnrs += 1
				avg_psnr = (avg_psnr * (n_psnrs - 1) + psnr) / n_psnrs

			print table_line
		print "-"*79
		print
		print "Average psnr:", avg_psnr, "db"
		print

main()
