aux_source_directory(. SRC)
aux_source_directory(resp RESP)
aux_source_directory(utils UTILS)
aux_source_directory(network NETWORK)
target_sources(reddish_hermit PRIVATE ${SRC} ${RESP} ${UTILS} ${NETWORK})