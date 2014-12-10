% This is yours... but here is a start...

clear;
%close all;

debug = 2;  % How much debug info for trainStump to produce.

if ~exist('rescaleImageVectors','file')
  addpath('./util');
end

%  Check path.  If these aren't on your path, then you will need
%  to add these toolboxes to your path.  See the startup file used to
% include the toolbox paths (as explained earlier in the course).
which showIm          % should be iseToolbox\pyrTools\showIm.m

%%%%%%%%  Initialize random number generator %%%%%%%%%%%%%%%%%%%%%%%
% Random number generator seed:
seed = round(sum(1000*clock));
rand('state', seed);
seed0 = seed;

%%%%%%%%%%% Load eye and non-eye images %%%%%%%%%%%%%
load('trainSet');
load('adaFit');
nTarg = size(eyeIm,2);
nNon = size(nonIm, 2);
testImSz = sizeIm;

%% %%%%%%%%%
% Do brightness normalization
% %%%%%%%%%%
respEye = rescaleImageVectors(eyeIm);
respNon = rescaleImageVectors(nonIm);

%% Put into format required for classification training and testing
X = [respEye respNon];
y = [ones(1, nTarg) zeros(1, nNon)];
wghts = ones(1, nTarg+nNon);

% Grab a derivative of gaussian filter
help buildGaussFeat
pFeat.sz = testImSz;
pFeat.sigma = 2;
pFeat.deriv = 1;
pFeat.theta = pi/4; 
pFeat.abs = false;
x0 = 5; 
y0 = 15;
pFeat.center = [x0 y0];
            
f = buildGaussFeat(pFeat);

if debug
  figure(1); clf; 
  showIm(f);
  pause(0.1);
end

[pFeat.err, pFeat.thres, pFeat.parity, H] = ...
    trainStump(f(:), pFeat.abs, X, y, wghts, debug);

for i = 1 : nFeatures
    pF = featList(i);
    f = buildGaussFeat(pF);
    if debug
      figure(1); clf; 
      showIm(f);
      pause(0.1);
    end

    [pFeat.err, pFeat.thres, pFeat.parity, H] = ...
        trainStump(f, pF.abs, X, y, wghts, debug);  
    pause;
end

% Blah blah...
