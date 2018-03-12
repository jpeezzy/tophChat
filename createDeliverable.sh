#!/bin/bash
# used for creating releases
# current releases BETA

# binary file 
EXECUTABLE_NAME='chat'
RELEASE_FOLDER='./beta_release'

CUSTOMER_TAR_FILE='Chat_Beta.tar.gz'
SRC_TAR_FILE='Chat_Beta_src.tar.gz'

SRC_DIR='beta_release/Chat_Beta_src' 
SRC_DIR_NAME='Chat_Beta_src'

CUSTOMER_DIR='beta_release/Chat_Beta/'
CUSTOMER_DIR_NAME='Chat_Beta'

#cp $(EXECUTABLE_NAME) $(SRC_DIR)/bin/ $(CUSTOMER_DIR)/bin/ -v
cp *.c ${SRC_DIR}/src/ -v
cp *.h ${SRC_DIR}/src/ -v
cp Makefile ${SRC_DIR}/src/ -v

tar -C ${RELEASE_FOLDER} -czvf ./${RELEASE_FOLDER}/${CUSTOMER_TAR_FILE} Chat_Beta
tar -C ${RELEASE_FOLDER} -czvf ./${RELEASE_FOLDER}/${SRC_TAR_FILE} Chat_Beta_src

