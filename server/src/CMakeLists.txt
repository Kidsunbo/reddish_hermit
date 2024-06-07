aux_source_directory(. SRC)
aux_source_directory(storage STORAGE)
aux_source_directory(kiedis KIEDIS)
target_sources(reddish_hermit_server PRIVATE ${SRC} ${STORAGE} ${KIEDIS})