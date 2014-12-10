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
    
  F = zeros(nFeatures,size(X,1));
  for i = 1:nFeatures
    f = buildGaussFeat(featList(i));
    F(i,:) = f(:);
  end
  featList = featList(1:nFeatures);
  
  P = [featList.parity]';
  R = F * X;
  absIdx = [featList.abs];
  R(absIdx,:) = abs(R(absIdx,:));
  R = bsxfun(@times,R,P);
  Th = [featList.thres]' .* P;
  H = bsxfun(@le,R,Th);
  Alpha = [featList.alpha];
  resp = Alpha * (2 .* H - 1);
  return;
