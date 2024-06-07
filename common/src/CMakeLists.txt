aux_source_directory(command COMMAND)
aux_source_directory(connection CONNECTION)
aux_source_directory(protocol PROTOCOL)
aux_source_directory(utils UTILS)

target_sources(reddish_hermit_common PRIVATE ${COMMAND} ${PROTOCOL} ${CONNECTION} ${UTILS})