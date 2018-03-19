#!/bin/bash
# used for creating releases
# current releases BETA

# binary file 
EXECUTABLE_NAME='chat'
RELEASE_FOLDER='./Chat_V1.0'

CUSTOMER_TAR_FILE='Chat_V1.0.tar.gz'
SRC_TAR_FILE='Chat_V1.0_src.tar.gz'

SRC_DIR='Chat_V1.0/Chat_V1.0_src' 
SRC_DIR_NAME='Chat_V1.0_src'

CUSTOMER_DIR='Chat_V1.0/Chat_V1.0/'
CUSTOMER_DIR_NAME='Chat_V1.0'

#cp $(EXECUTABLE_NAME) $(SRC_DIR)/bin/ $(CUSTOMER_DIR)/bin/ -v
cp *.c ${SRC_DIR}/src/ -v
cp *.h ${SRC_DIR}/src/ -v
cp Makefile ${SRC_DIR}/src/ -v

tar -C ${RELEASE_FOLDER} -czvf ./${RELEASE_FOLDER}/${CUSTOMER_TAR_FILE} Chat_V1.0
tar -C ${RELEASE_FOLDER} -czvf ./${RELEASE_FOLDER}/${SRC_TAR_FILE} Chat_V1.0_src

