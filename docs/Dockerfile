# (c) 2022 Copyright, Real-Time Innovations, Inc.  All rights reserved.
# No duplications, whole or partial, manual or electronic, may be made
# without express written permission.  Any such copies, or revisions thereof,
# must display this notice unaltered.
# This code contains trade secrets of Real-Time Innovations, Inc.
FROM quay.io/pypa/manylinux2014_x86_64
ARG USER_NAME=jenkins
ARG USER_UID

RUN adduser $USER_NAME -u $USER_UID
COPY requirements.txt .
RUN /opt/python/cp36-cp36m/bin/pip install -r requirements.txt

ENV PATH=/home/jenkins/.local/bin:/opt/python/cp36-cp36m/bin:$PATH