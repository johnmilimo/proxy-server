#!/bin/bash

lsof -i tcp:8088 | grep -v PID | awk '{print $2}' | xargs kill && gcc server.c proxy.c utils.c -o server && ./server