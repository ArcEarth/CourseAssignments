clear
load('adaFit');
load('trainSet');

testImSz = sizeIm;
% Do brightness normalization
trainTarg = rescaleImageVectors(eyeIm);
trainNon = rescaleImageVectors(nonIm);
nTarg = size(trainTarg,2);
nNon = size(trainNon, 2);

% 
% figure(5); clf;
% sumX = sum(trainTarg,2) ./ nTarg;
% showIm(reshape(sumX,sizeIm));
% title('Average Eye');

figure(3); clf;
% hold on;
% hx = logspace(-1,2);
% hy = exp(hx);
% loglog(hx,hy);
% hold on;

X = [trainTarg trainNon];
y = [ones(1,nTarg) zeros(1,nNon)];



% Training Set
[hx,hy,~] = calculateDET(featList, nFeatures, X, y);
plots = loglog(hx*100, hy*100);
M = 'TrainDT';
grid on;
hold on;

% Test Set
load('testSet');
nTarg = size(eyeIm,2);
nNon = size(nonIm, 2);
testTarg = rescaleImageVectors(testEyeIm);
testNon = rescaleImageVectors(testNonIm);

X = [testTarg testNon];
y = [ones(1,nTarg) zeros(1,nNon)];

for F = 10 : 10 : nFeatures
    [hx,hy,tau] = calculateDET(featList, F, X, y);
    plots = [plots ; loglog(hx*100, hy*100)];
    M = [M ; sprintf('M = %3d',F)];
end

legend(plots,M);

title(sprintf('DET for Adaboost, nFeat = %d',nFeatures));
xlabel('False Alarm Rate (%)');
ylabel('Miss Rate (%)');
%hold on;
