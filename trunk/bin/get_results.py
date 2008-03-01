# Imports
import os
import re
import string

# Constants
wic_tool_path	= "wictool.exe"

images_dir		= "../res/images/"
working_dir		= "./tmp/"

images			= ["lenaD.bmp", "barbaraD.bmp", "goldhillD.bmp"]
filters			= ["Antonini", "cdf97", "Petuhov1"]
bpps			= [0.25, 0.5, 1.0]

# Creates regular expression string to match float field
def re_float_field(name):
	return "([ \t]*" + name + ":[ \t]*)(?P<" + name + ">[-+]?[0-9]*\.?[0-9]+)"
	
# Composes encode options string -----------------------------------------------
def mk_encode_opts(*args, **vals):
	opts = " -v -e"
	if "filter" in vals:opts += " -f " + str(vals["filter"])
	if "method" in vals: opts += " -m " + str(vals["method"])
	if "q" in vals: opts += " -q " + str(vals["q"])
	if "l" in vals: opts += " -l " + str(vals["l"])
	if "steps" in vals: opts += " -s " + str(vals["steps"])
	if "bpp" in vals: opts += " -b " + str(vals["bpp"])

	return opts

# Decodes wic_file into dest_file ----------------------------------------------
def decode_image(wic_file, dest_image):
	cmd_line = wic_tool_path  + " -v -d " + wic_file + " " + dest_image;
	f = os.popen(cmd_line, "r")

	if not f: return -1

	return 0

# Encodes src_image into wic_file
def encode_image(src_image, wic_file, channel="r", *args, **vals):
	src_str = " " + src_image + ":" + channel;
	wic_str = " " + wic_file;
	cmd_line = wic_tool_path + mk_encode_opts(**vals) + src_str + wic_str;

	f = os.popen(cmd_line, "r")

	if not f: return None

	result = {}

	for line in f:
		m = re.compile(re_float_field("bpp")).search(line)
		if m: result["bpp"] = float(m.group("bpp"))

		m = re.compile(re_float_field("size")).search(line)
		if m: result["size"] = float(m.group("size"))

		m = re.compile(re_float_field("size")).search(line)
		if m: result["size"] = float(m.group("size"))

		m = re.compile(re_float_field("q")).search(line)
		if m: result["q"] = float(m.group("q"))

		m = re.compile(re_float_field("lambda")).search(line)
		if m: result["l"] = float(m.group("lambda"))

	return result
	

# Calculates psnr using wictool ------------------------------------------------
def calc_psnr(image1, image2, channel="r"):
	# Compose command line
	img1_str	= " " + image1 + ":" + channel
	img2_str	= " " + image2 + ":" + channel
	cmd_line = wic_tool_path  + " -p" + img1_str + img2_str

	# Run wictool to calc psnr
	f = os.popen(cmd_line, "r")

	# Error
	if not f: return -1

	# Search for psnr in wictool output
	for line in f:
		m = re.compile("([ \t]*psnr:[ \t]*)(?P<psnr>[-+]?[0-9]*\.?[0-9]+)").search(line)
		if m: return float(m.group("psnr"))

	# Error
	return -1

# 
#def 

# Main function
def main():
	try:
		os.makedirs(working_dir)
	except:
		pass
	
	for filter in filters:
		# Print table head
		print filter + ":"
		print "-"*79
		fw = 16
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
				table_line += "|" + str(psnr).center(fw)
			print table_line
		print "-"*79
		print

main()
