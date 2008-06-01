# Imports
import os
import re
import string
import pylab
import copy

# Constants
wic_tool_path	= "wictool.exe"
wic_tool_path_d	= "wictool_d.exe"
wic_tool_path_r	= "wictool_r.exe"

# Creates regular expression string to match float field
def re_float_field(name):
	return "([ \t]*" + name + ":[ \t]*)(?P<" + name + ">[-+]?[0-9]*\.?[0-9]+)"

def re_acm_stat_begin(mode):
	return "acoder::" + mode + "::model::(?P<no>[0-9]+)::(?P<size>[0-9]+)::(?P<delta>[-+]?[0-9]+)::(?P<abs_avg>[0-9]+)"

def re_acm_stat_value(mode):
	return "acoder::" + mode + "::value::(?P<no>[0-9]+)::(?P<i>[-+]?[0-9]+)::(?P<value>[-+]?[0-9]+)"
	

# Class that implements acoder model statistic
class acm_stats:
	def __init__(self, no, size, delta, abs_avg):
		self.no			= no
		self.size		= size
		self.delta		= delta
		self.stats		= [0]*size
		self.abs_avg	= abs_avg
	def set(self, i, p):
		self.stats[i] = p
	def setr(self, i, p):
		self.stats[i - self.delta] = p
	def show(self):
		print "no: " + str(self.no) + "; size: " + str(self.size) + "; delta: " + str(self.delta)+ "; abs_avg: " +str(self.abs_avg)
		print "stats:", self.stats
	def sub(self, other):
		for i in range(0, min(len(self.stats), len(other.stats))):
			self.stats[i] = self.stats[i] - other.stats[i]

# Composes encode options string -----------------------------------------------
def mk_encode_opts(*args, **vals):
	opts = " -v -e"
	if "filter" in vals: opts += " -f " + str(vals["filter"])
	if "method" in vals: opts += " -m " + str(vals["method"])
	if "q" in vals: opts += " -q " + str(vals["q"])
	if "l" in vals: opts += " -l " + str(vals["l"])
	if "steps" in vals: opts += " -s " + str(vals["steps"])
	if "bpp" in vals: opts += " -b " + str(vals["bpp"])

	return opts

# Decodes wic_file into dest_file ----------------------------------------------
def decode_image(wic_file, dest_image, *args, **vals):
	if "wic_tool_path" in vals:
		_wic_tool_path = str(vals["wic_tool_path"])
	else:
		_wic_tool_path = wic_tool_path

	cmd_line = _wic_tool_path  + " -v -d " + wic_file + " " + dest_image;
	f = os.popen(cmd_line, "r")

	if not f: return -1

	return 0

# Encodes src_image into wic_file
def encode_image(src_image, wic_file, channel="r", verbose=None, *args, **vals):
	if "wic_tool_path" in vals:
		_wic_tool_path = str(vals["wic_tool_path"])
	else:
		_wic_tool_path = wic_tool_path

	src_str = " " + src_image + ":" + channel;
	wic_str = " " + wic_file;
	cmd_line = _wic_tool_path + mk_encode_opts(**vals) + src_str + wic_str;
	
	# print "encode_image: \"" + cmd_line + "\""

	f = os.popen(cmd_line, "r")

	if not f: return None

	result = {}

	acs_init = {}
	acs_encd = {}

	for line in f:
		if verbose: print line

		m = re.compile(re_float_field("bpp")).search(line)
		if m: result["bpp"] = float(m.group("bpp"))

		m = re.compile(re_float_field("size")).search(line)
		if m: result["size"] = float(m.group("size"))

		m = re.compile(re_float_field("q")).search(line)
		if m: result["q"] = float(m.group("q"))

		m = re.compile(re_float_field("lambda")).search(line)
		if m: result["l"] = float(m.group("lambda"))

		m = re.compile(re_acm_stat_begin("init")).search(line)
		if m:
			no				= int(m.group("no"))
			size			= int(m.group("size"))
			delta			= int(m.group("delta"))
			abs_avg			= int(m.group("abs_avg"))
			acs_init[no]	= acm_stats(no, size, delta, abs_avg)

		m = re.compile(re_acm_stat_value("init")).search(line)
		if m:
			no				= int(m.group("no"))
			i				= int(m.group("i"))
			p				= int(m.group("value"))
			acs_init[no].set(i, p)

		m = re.compile(re_acm_stat_begin("encd")).search(line)
		if m:
			no				= int(m.group("no"))
			size			= int(m.group("size"))
			delta			= int(m.group("delta"))
			abs_avg			= int(m.group("abs_avg"))
			acs_encd[no]	= acm_stats(no, size, delta, abs_avg)

		m = re.compile(re_acm_stat_value("encd")).search(line)
		if m:
			no				= int(m.group("no"))
			i				= int(m.group("i"))
			p				= int(m.group("value"))
			acs_encd[no].set(i, p)

	result["acs_init"] = acs_init;
	result["acs_encd"] = acs_encd;

	return result

# Get acoder statistics
def plot_acoder_stats(acs_init, acs_encd, *args, **vals):

	pylab.clf()

	if "models_range" in vals:
		models_range = vals["models_range"]
	else:
		models_range = []
		for item in acs_init.keys():
			if item in acs_encd.keys():
				models_range.append(item)

	# Plot initialization stats
	pylab.subplot(2, 1, 1)
	for i in models_range:	#acs_init:
		if not i in acs_init:
			continue
		stats = copy.deepcopy(acs_init[i].stats)
		a = min(stats)
		b = max(stats)
		d = abs(b - a) + 1
		for j in range(0, len(stats)):
			stats[j] = 100*stats[j] / d

		pylab.plot(range(-acs_init[i].delta, acs_init[i].size - acs_init[i].delta),
				   stats, '--')#, antialiased=False, lw=0.5)

	legend = []
	for i in models_range:	#acs_init:
		if not i in acs_init:
			continue
		legend.append(str(i) + " - "+ str(max(acs_init[i].stats)))
	pylab.legend(tuple(legend))

	# Plot encoded stats
	pylab.subplot(2, 1, 2)
	for i in models_range:	#acs_encd:
		if not i in acs_encd:
			continue
		stats = copy.deepcopy(acs_encd[i].stats)
		a = min(stats)
		b = max(stats)
		d = abs(b - a) + 1
		for j in range(0, len(stats)):
			stats[j] = 100*stats[j] / d

		pylab.plot(range(-acs_encd[i].delta, acs_encd[i].size - acs_encd[i].delta),
				   stats)#, antialiased=False, lw=0.5)
		pylab.legend(str(a))

	legend = []
	for i in models_range:	#acs_encd:
		if not i in acs_encd:
			continue
		legend.append(str(i) + " - "+ str(max(acs_encd[i].stats)))
	pylab.legend(tuple(legend))

	# Some comments about graph
	if "caption" in vals:
		pylab.title(str(vals["caption"]))

	if "save_path" in vals:
		pylab.savefig(str(vals["save_path"]))
	else:
		pylab.show()

	# Clear figure
	#pylab.clf()



# Writes acoder stats into file
def write_acoder_stats(acs_init, acs_encd, *args, **vals):
	
	#if "models_range" in vals:
	#	models_range = vals["models_range"]
	#else:
	#	models_range = []
	#	for item in acs_init.keys():
	#		if item in acs_encd.keys():
	#			models_range.append(item)

	# Write initialization stats
	if "save_path" in vals:
		f = open(vals["save_path"], "w")
	else:
		f = sys.stdout

	f.write(80*"-" + "\n")

	if "about" in vals:
		f.write("About: " + str(vals["about"]) + "\n\n")

	f.write("[Initialization]\n\n")
	for i in acs_init:
		mdlMin	= -acs_init[i].delta
		mdlMax	= acs_init[i].size - acs_init[i].delta
		stats	= acs_init[i].stats
		abs_avg	= acs_init[i].abs_avg

		f.write("Model " + str(i) + " [" + str(mdlMin) + ", " + str(mdlMax)+ "], abs_avg=" + str(abs_avg) + ":\n")

		for j in range(0, len(stats)):
			if 0 != j:
				f.write(", ")
			f.write(str(stats[j]))

		f.write("\n\n");

	f.write("[/Initialization]\n\n\n")

	f.write("[Encoded]\n\n")
	for i in acs_encd:
		mdlMin	= -acs_encd[i].delta
		mdlMax	= acs_encd[i].size - acs_encd[i].delta
		stats	= acs_encd[i].stats

		f.write("Model " + str(i) + " [" + str(mdlMin) + ", " + str(mdlMax)+ "]:\n")

		for j in range(0, len(stats)):
			if 0 != j:
				f.write(", ")
			f.write(str(stats[j]))

		f.write("\n\n");

	f.write("[/Encoded]\n\n\n")



# Calculates psnr using wictool ------------------------------------------------
def calc_psnr(image1, image2, channel="r", *args, **vals):
	if "wic_tool_path" in vals:
		_wic_tool_path = str(vals["wic_tool_path"])
	else:
		_wic_tool_path = wic_tool_path

	# Compose command line
	img1_str	= " " + image1 + ":" + channel
	img2_str	= " " + image2 + ":" + channel
	cmd_line = _wic_tool_path  + " -p" + img1_str + img2_str

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
