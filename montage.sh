#!/bin/bash

montage -geometry '1x1+0+0<' -adjoin regiona-1_0.png regiona0_0.png regiona-1_-1.png regiona0_-1.png regiona.png
montage -geometry '1x1+0+0<' -adjoin regionr-1_0.png regionr0_0.png regionr-1_-1.png regionr0_-1.png regionr.png
montage -geometry '1x1+0+0<' -adjoin regiont-1_0.png regiont0_0.png regiont-1_-1.png regiont0_-1.png regiont.png
montage -geometry '1x1+0+0<' -adjoin regionh-1_0.png regionh0_0.png regionh-1_-1.png regionh0_-1.png regionh.png

rm regiona-1_0.png regiona0_0.png regiona-1_-1.png regiona0_-1.png regionr-1_0.png regionr0_0.png regionr-1_-1.png regionr0_-1.png regiont-1_0.png regiont0_0.png regiont-1_-1.png regiont0_-1.png regionh-1_0.png regionh0_0.png regionh-1_-1.png regionh0_-1.png
