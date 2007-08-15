include Makefile.config

arcoder_dir	:= $(PROJ_ROOT)/arcoder
imgs_dir	:= $(PROJ_ROOT)/imgs
doc_dir		:= $(PROJ_ROOT)/doc
cdf97_dir	:= $(PROJ_ROOT)/wavelets/cdf97
libwic_dir	:= $(PROJ_ROOT)/src/libwic
cwrap_dir	:= $(PROJ_ROOT)/src/cwrap
qatest_dir	:= $(PROJ_ROOT)/src/qatest
wictool_dir	:= $(PROJ_ROOT)/src/wictool

all_targets	:=		\
			arcoder \
			doc	\
			cdf97	\
			libwic	\
			wictool
#			imgs
#			cwrap
#			qatest

all_target_dirs	:= $(foreach dirname, $(addsuffix _dir, $(all_targets)), $(value $(dirname)))

.PHONY: $(all_targets)

all:
	@for curr_dir in $(all_target_dirs);	\
	do					\
		if ! $(MAKE) -C $$curr_dir;	\
		then				\
			break;			\
		fi				\
	done

distclean clean:
	@for curr_dir in $(all_target_dirs);	\
	do					\
		$(MAKE) -C $$curr_dir clean;	\
	done

