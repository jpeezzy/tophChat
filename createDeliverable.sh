#!/bin/bash
# used for creating releases
# current releases ALPHA

# binary file 
EXECUTABLE_NAME='chat'
RELEASE_FOLDER='./alpha_release'

CUSTOMER_TAR_FILE='Chat_Alpha.tar.gz'
SRC_TAR_FILE='Chat_Alpha_src.tar.gz'

SRC_DIR='alpha_release/Chat_Alpha_src/' 
SRC_DIR_NAME='Chat_Alpha_src'

CUSTOMER_DIR='alpha_release/Chat_Alpha/'
CUSTOMER_DIR_NAME='Chat_Alpha'

#cp $(EXECUTABLE_NAME) $(SRC_DIR)/bin/ $(CUSTOMER_DIR)/bin/ -v
cp *.c ${SRC_DIR}/src/ -v
cp *.h ${SRC_DIR}/src/ -v
cp Makefile ${SRC_DIR}/src/ -v

tar -C ${RELEASE_FOLDER} -czvf ./${RELEASE_FOLDER}/${CUSTOMER_TAR_FILE} Chat_Alpha
tar -C ${RELEASE_FOLDER} -czvf ./${RELEASE_FOLDER}/${SRC_TAR_FILE} Chat_Alpha_src

