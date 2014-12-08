%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  File: trainAdaGauss.m
%  Matlab script file
%  Date: Oct, 08
%

% Dependencies, Toolboxes:
%      iseToolbox/
%      utvisToolbox/file/
% Data files: trainSet.mat testSet.mat
% Writes data file: adaFit.mat

% Author: YOU and ADJ
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%% Check Path and Constants  %%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
clear;
%close all;

debugFish = 0;          % Set to 1 or 2 to debug fishFeature
debugTrainStump = 1;    % Set to 1 or 2 to debug trainStump
maxFeatures = 20;       % Maximum number of features in strong classifier
maxErr = 0.4;           % Ignore any feature with err > maxErr
greedyErr = 0.3;        % Use any feature found with err < greedyErr
% This breaks out of the search over all features, and the selected
% feature may not be the optimal next feature.

sigmaSet = [4 2 1];     % The gaussian derivative sigmas (coarse to fine)
                        % to try.
                        
%% Check path
if ~exist('rescaleImageVectors','file')
  addpath('./util');
end

%  Check for toolboxes.
%  If these aren't on your path, then you will need
%  to add these toolboxes to your path.  See ~jepson/pub/matlab/startup.m
which showIm          % should be iseToolbox\pyrTools\showIm.m

%%%%%%%%  Initialize random number generator %%%%%%%%%%%%%%%%%%%%%%%
% Random number generator seed:
seed = round(sum(1000*clock));
rand('state', seed);
seed0 = seed;

%%%%%%%%%%% Load eye and non-eye images %%%%%%%%%%%%%%%%%%%%%%%%%%
load('trainSet');
nTarg = size(eyeIm,2);
nNon = size(nonIm, 2);
testImSz = sizeIm;

%% %%%%%%%%%
% Do brightness normalization
% %%%%%%%%%%
testTarg = rescaleImageVectors(eyeIm);
testNon = rescaleImageVectors(nonIm);


% Form training data
X = [testTarg testNon];
y = [ones(1,nTarg) zeros(1,nNon)];

% Initial weights, scaled so that sum of the target weights is 0.5,
% as is the sum of the non target weights.
wghts = [ones(1,nTarg)/(2*nTarg), ones(1,nNon)/(2*nNon)];

nFeatures = 0;

while nFeatures < maxFeatures

  bestFeat = fishFeature(testImSz, sigmaSet, X, y, wghts, greedyErr, ...
                         debugFish);

  f = buildGaussFeat(bestFeat);

  figure(1); clf; 
  showIm(f);
  pause(0.1);
  [err, thres, parity, H] = ...
      trainStump(f(:), bestFeat.abs, X, y, wghts, debugTrainStump);
  
  err
  if err > maxErr
    break;
  end
  
  bestFeat.alpha = log((1-err)/err);
  
  % Add bestFeat to feature list
  if nFeatures == 0
    featList(1) = bestFeat;
    nFeatures = 1;
  else
    nFeatures = nFeatures+1;
    featList(nFeatures) = bestFeat;
  end
  
  wghts = doAdaBoostStep(wghts, bestFeat.alpha, H, y);
  
  % Check responses
  resp = evalBoosted(featList, nFeatures, X);
  
  % Histogram of boosted detector responses
  if (any(y >0) & any(y == 0))
    [hTarg, hxTarg] = histo(resp(y>0), 101);
    hTarg = hTarg/(sum(hTarg) * (hxTarg(2) - hxTarg(1)));
    [hNon, hxNon] = histo(resp(y==0), 101);
    hNon = hNon/(sum(hNon) * (hxNon(2) - hxNon(1)));

    figure(2); clf;
    plot(hxTarg, hTarg, 'g');
    hold on;
    plot(hxNon, hNon, 'r');
    title(sprintf('Hist of Boosted Responses, Targ/Non (g/r), nFeat = %d', ...
                  nFeatures));
    xlabel('Response');
    ylabel('Probability');
    pause(0.1);
  
    fprintf('True/False Pos. Rates: %f, %f\n', ...
            sum(resp(y>0) >0)/sum(y>0), sum(resp(y==0) >0)/sum(y==0));
  end
  
end


save('adaFit.mat', 'nFeatures', 'featList');

pause;

% The rest of the code is yours....

clear
load('adaFit');

load('testSet');
testImSz = sizeIm;

%% %%%%%%%%%
% Do brightness normalization
% %%%%%%%%%%
testTarg = rescaleImageVectors(testEyeIm);
testNon = rescaleImageVectors(testNonIm);
nTarg = size(testTarg,2);
nNon = size(testNon, 2);
