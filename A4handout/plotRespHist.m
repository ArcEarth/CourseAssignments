clear
load('adaFit');
load('trainSet');

testImSz = sizeIm;
% Do brightness normalization
trainTarg = rescaleImageVectors(eyeIm);
trainNon = rescaleImageVectors(nonIm);
nTarg = size(trainTarg,2);
nNon = size(trainNon, 2);

X = [trainTarg trainNon];
y = [ones(1,nTarg) zeros(1,nNon)];
resp = evalBoosted(featList, 10, X);

% Histogram of boosted detector responses
if (any(y >0) & any(y == 0))
    [hTarg, hxTarg] = histo(resp(y>0), 101);
    hTarg = hTarg/(sum(hTarg) * (hxTarg(2) - hxTarg(1)));
    [hNon, hxNon] = histo(resp(y==0), 101);
    hNon = hNon/(sum(hNon) * (hxNon(2) - hxNon(1)));

    figure(3); clf;
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