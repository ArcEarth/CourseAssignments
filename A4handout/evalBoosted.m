function [resp] = evalBoosted(featList, nFeatures, X)
  % [resp] = evalBoosted(featList, nFeatures, X)
  % Evaluate continous valued strong classifier based
  % on a list of weighted weak classifiers.
  % Input:
  %  featList  F dimensional array of gaussian derivative
  %            feature structures (see buildGaussFeat).
  %            Here F >= nFeatures.  Also
  %            pFeat = featList(k) is a suitable argument
  %            for buildGaussFeat.m. Also, we assume pFeat.thres
  %            pFeat.parity and pFeat.alpha have all
  %            been set, as explained in the assignment handout.
  %  nFeatures  the number of features to use 1<= nFeatures <= F
  %            This is M in eqn (3) the assignment handout.
  %  X  the N x K data matrix.  Each column is one data item.
  % Output:
  %  resp - double 1 X K vector, with continuous valued strong classifier
  %         responses S_M in (3) in the handout, where M = nFeatures
    

  resp = zeros(1, size(X,2));

  return;
