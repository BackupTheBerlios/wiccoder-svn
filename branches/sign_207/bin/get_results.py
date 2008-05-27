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
#images			= ["lenaD.bmp"]
# filters			= ["Antonini", "cdf97", "Petuhov1"]
filters			= ["Antonini", "Petuhov1"]
#filters			= ["Petuhov1"]
bpps			= [0.25, 0.5, 1.0]
#bpps			= [0.5]


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
					stats_caption 	= image + "; " + filter + "; bpp: " + str(bpp) + " (q=" + str(r["q"]) + ", l=" + str(r["l"]) + ")"
					wic_stats_file 	= string.join([working_dir + image, filter, str(bpp), "stats", "wic"], ".")
					dest_stats_image= string.join([working_dir + image, filter, str(bpp), "stats", "bmp"], ".")
					stats_save_path = string.join([working_dir + image, filter, str(bpp), "stats", "png"], ".")

					stats_r = encode_image(src_image, wic_stats_file, filter=filter, method="manual", q=r["q"], l=r["l"], wic_tool_path=wic_tool_path_r)
					decode_image(wic_stats_file, dest_stats_image, wic_tool_path=wic_tool_path_r)
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
