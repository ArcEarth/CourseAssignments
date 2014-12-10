function [err, thres, parity, stumpResp, r] = trainStump(f, useAbs, X, y, ...
                                                  w, debug)
  %[err, thres, parity, stumpResp, r] = trainStump(f, useAbs, X, y, ...
  %                                                w, debug)
  % Input:
  %  f - N dimensional projection vector.
  %  useAbs - boolean, whether to use f(:)' * X or abs(f(:)' * X) as
  %     the feature.
  %  X  - N x K training matrix.
  %  y  - 1 x K label matrix, with y(k) = 1 indicating target, 0
  %       non-target.
  %  w  - 1 x K vector of weights.  Precondition: w(k) >= 0, sum_k w(k) > 0
  %  debug = 0, 1, or 2 for no info, histogram plot, more info,
  %         respectively.
  % Output
  %  err - (double) minimum weighted error of weak classifier (see (2) in
  %        the assignment handout).
  %  thres - (double) the optimum threshold theta_1 in (1) of the assignment.
  %  parity - (-1 or 1) the optimum parity theta_2 in (1) of the assignment.
  %  stumpResp - binary, 1 by K, optimum weak classifier response on X,
  %             i.e., the value of h(x,theta) in (1) of the assignment, 
  %             for the optimum parameters theta.
  %  r  - double 1 by K, the continuous valued response u(f(:)' * X)
  %       used by the optimum weak classifier (see (1) in the assignment).

  sumW = sum(w);
  
  sumWNon = sum(w .* (y == 0));
  
  r = f(:)' * X;
  if useAbs
    r = abs(r);
  end
  
  if debug & any(y > 0) & any(y == 0)
    % Plot histogram of (unweighted) responses.
    [hTarg, hxTarg] = histo(r(y>0), 101);
    hTarg = hTarg/(sum(hTarg) * (hxTarg(2) - hxTarg(1)));
    [hNon, hxNon] = histo(r(y==0), 101);
    hNon = hNon/(sum(hNon) * (hxNon(2) - hxNon(1)));
    figure(2); clf;
    plot(hxTarg, hTarg, 'g');
    hold on;
    plot(hxNon, hNon, 'r');
    xlabel('Response');
    ylabel('Probability');
    pause(0.1);
  end
  
  %%%%%% YOU NEED TO FILL IN CODE HERE
  %% You can change anything in this M-file (except the parameters
  %% and return values)
%   r = [4 5 1 2 3];
%   y = [0 0 1 1 1];
%   w = ones(1,5);
  
  K = size(r,2);
  [ran,IX] = sort(r);
  ran = [ran(1)-1 ran ran(K)+1];
  ybackup = y;
  y = y(IX);
  w = w(IX);
  thress = zeros(2,1);
  err = zeros(2,1);
  
  ls = cumsum(y .* w);
  rs = fliplr(cumsum(fliplr(~y .* w)));
  bs = [0 ls] + [rs 0];
  [err(1),idx] = min(bs);
  thress(1) = 0.5*(ran(idx) + ran(idx+1));
 
  ls = cumsum(~y .* w);
  rs = fliplr(cumsum(fliplr(y .* w)));
  bs = [0 ls] + [rs 0];
  [err(2),idx] = min(bs);
  thress(2) = 0.5*(ran(idx) + ran(idx+1));
  
  [err,parity] = min(err);
  err = err / sumW;
  thres = thress(parity);
  parity = sign(parity - 1.5);
  if (parity <0)
    stumpResp = r > thres;
  else
    stumpResp = r <= thres;
  end
  
%% Plot histogram

if debug
    % Plot threshold on histogram figure
    y = ybackup;
    tpRate = sum(stumpResp(y>0))/sum(y>0);
    fpRate = sum(stumpResp(y==0))/sum(y==0);  
    figure(2); hold on;
    ax = axis;
    plot([thres, thres], ax(3:4), 'k');
    title(sprintf('Hist. of Weak Classifier, Targ/Non (g/r), Err=%0.2f, TPR=%0.2f, FPR=%0.2f',err,tpRate,fpRate));
    pause(0.1);
  end

  return;

  
