# basic makefile

# paths (can also pass via command line)
MKDIR	?=	mkdir # should support POSIX '-p'
MWERKS	?=	# mwcc path here (should also have mwld in the same directory)
WINE	?=	# wine path here if not building on Windows natively

# targets
.PHONY: help libnw4r_snd libnw4r_sndD clean

help:
	@echo 'options:'
	@echo '    libnw4r_snd            creates libnw4r_snd.a (release)'
	@echo '    libnw4r_sndD           creates libnw4r_sndD.a (debug)'
	@echo
	@echo '    build/release/<file>.o compiles <file>.o (release)'
	@echo '    build/debug/<file>.o   compiles <file>.o (debug)'
	@echo
	@echo '    clean                  cleans up build artifacts'

libnw4r_snd: lib/libnw4r_snd.a
libnw4r_sndD: lib/libnw4r_sndD.a
clean:; -@rm -rf build/ lib/

# system include search directories
export MWCIncludes = include:include/stdlib

# flags
flags_main = -proc gekko -fp hardware -lang c99 -enum int -cpp_exceptions off \
	-cwd include -enc UTF-8 -flag no-cats

flags_opt_debug = -opt off -inline off -g
flags_opt_release = -O4,p -ipa file -DNDEBUG

# source files
nw4r_snd_src :=	\
	snd_AnimSound.cpp				\
	snd_AxManager.cpp				\
	snd_AxVoice.cpp					\
	snd_AxVoiceManager.cpp			\
	snd_Bank.cpp					\
	snd_BankFile.cpp				\
	snd_BasicPlayer.cpp				\
	snd_BasicSound.cpp				\
	snd_BiquadFilterPreset.cpp		\
	snd_Channel.cpp					\
	snd_debug.cpp					\
	snd_DisposeCallbackManager.cpp	\
	snd_DvdSoundArchive.cpp			\
	snd_EnvGenerator.cpp			\
	snd_ExternalSoundPlayer.cpp		\
	snd_InstancePool.cpp			\
	snd_Lfo.cpp						\
	snd_McsSoundArchive.cpp			\
	snd_MemorySoundArchive.cpp		\
	snd_MidiSeqPlayer.cpp			\
	snd_MidiSeqTrack.cpp			\
	snd_MidiSeqTrackAllocator.cpp	\
	snd_MmlParser.cpp				\
	snd_MmlSeqTrack.cpp				\
	snd_MmlSeqTrackAllocator.cpp	\
	snd_PlayerHeap.cpp				\
	snd_SeqFile.cpp					\
	snd_SeqPlayer.cpp				\
	snd_SeqSound.cpp				\
	snd_SeqSoundHandle.cpp			\
	snd_SeqTrack.cpp				\
	snd_SoundActor.cpp				\
	snd_SoundArchive.cpp			\
	snd_SoundArchiveFile.cpp		\
	snd_SoundArchivePlayer.cpp		\
	snd_SoundHandle.cpp				\
	snd_SoundPlayer.cpp				\
	snd_SoundStartable.cpp			\
	snd_SoundSystem.cpp				\
	snd_SoundThread.cpp				\
	snd_StrmChannel.cpp				\
	snd_StrmFile.cpp				\
	snd_StrmPlayer.cpp				\
	snd_StrmSound.cpp				\
	snd_StrmSoundHandle.cpp			\
	snd_Task.cpp					\
	snd_TaskManager.cpp				\
	snd_TaskThread.cpp				\
	snd_Util.cpp					\
	snd_Voice.cpp					\
	snd_VoiceManager.cpp			\
	snd_WaveArchive.cpp				\
	snd_WaveFile.cpp				\
	snd_WaveSound.cpp				\
	snd_WaveSoundHandle.cpp			\
	snd_WsdFile.cpp					\
	snd_WsdPlayer.cpp				\
									\
	snd_adpcm.cpp

.PHONY: envcheck

envcheck:
ifeq ($(strip ${MWERKS}),)
	$(error MWERKS not set)
endif

# object files
build/debug/%.o: source/%.cpp | envcheck
	@${MKDIR} -p build/debug/$(dir $*)
	${WINE} ${MWERKS} ${flags_main} ${flags_opt_debug} ${FLAGS} -o $@ -c $<

build/release/%.o: source/%.cpp | envcheck
	@${MKDIR} -p build/release/$(dir $*)
	${WINE} ${MWERKS} ${flags_main} ${flags_opt_release} ${FLAGS} -o $@ -c $<

# snd_adpcm.o is a special case: it's -O4,p even on debug
build/debug/snd_adpcm.o: source/snd_adpcm.cpp | envcheck
	@${MKDIR} -p build/debug/$(dir $*)
	${WINE} ${MWERKS} ${flags_main} ${flags_opt_release} ${FLAGS} -o $@ -c $<

# library archives
.NOTPARALLEL: lib/libnw4r_sndD.a lib/libnw4r_snd.a

# NOTE: [R89JEL] does not use the D suffix; it might be fake
lib/libnw4r_sndD.a: $(foreach f,$(basename ${nw4r_snd_src}),build/debug/$f.o)
	@${MKDIR} -p lib/
	${WINE} ${MWERKS} ${flags_main} -o $@ -library $^

lib/libnw4r_snd.a: $(foreach f,$(basename ${nw4r_snd_src}),build/release/$f.o)
	@${MKDIR} -p lib/
	${WINE} ${MWERKS} ${flags_main} -o $@ -library $^
