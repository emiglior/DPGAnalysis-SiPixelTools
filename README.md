
# DPGAnalysis-SiPixelTools

Prepare your working directory with whatever CMSSW version you want to use (branch "master" is currently on 94X)

```
cmsrel CMSSW_10_2_0_pre6
cd CMSSW_10_2_0_pre6/src
cmsenv
```

Fork DPGAnalysis-SiPixelTools from https://github.com/cms-analysis/DPGAnalysis-SiPixelTools and checkout the code

```

git clone https://github.com/emiglior/DPGAnalysis-SiPixelTools PixelTools
cd PixelTools/
git fetch origin
git checkout -b DevelopmentBranch origin/SiPixelLorentzAngle_10_2_X
cd **myPackage**(see below)
scram b -j 20
```

**myPackage** can be any of the repositories below, containing code to calibrate & test pixels:

- PixelTriplets - measure hit resolution in bpix using the triplet method.

- PixelTrees - make pixel trees used for testing and calibration of resonstructin

- PixelHitAsociator - a customised version of the SimHit-RecHit comparion, used for resolution testing.

- HitAnalyser - various codes to test pixel simHits, digis, clusters and recHits.

- LA-Calibration - code to calibrate the LA from data.

- GainCalibration - code to run the gain calibartion to obtain pedestals and (offsets) gains (slopes) per pixel.

- PixelDBTools - various test programs to monitor the content of pixel DB payloads.
