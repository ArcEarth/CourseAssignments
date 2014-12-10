clear
load('adaFit');

%% Tranning set
figure(3); clf;
plots = [];
M = [];

load('trainSet');

testImSz = sizeIm;
% Do brightness normalization
trainTarg = rescaleImageVectors(eyeIm);
trainNon = rescaleImageVectors(nonIm);
nTarg = size(trainTarg,2);
nNon = size(trainNon, 2);

X = [trainTarg trainNon];
y = [ones(1,nTarg) zeros(1,nNon)];

for F = 10 : 10 : nFeatures
    [hx,hy,tau] = calculateDET(featList, F, X, y);
    plots = [plots ; loglog(hx*100, hy*100)];
    grid on;
    hold on;
    M = [M ; sprintf('M = %3d',F)];
end

legend(plots,M);

title(sprintf('DET for Adaboost (Trainning Set), Max Feature = %d',nFeatures));
xlabel('False Alarm Rate (%)');
ylabel('Miss Rate (%)');


%% Testing set
figure(4); clf;
plots = [];
M = [];

%Test Set
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
    grid on;
    hold on;
    M = [M ; sprintf('M = %3d',F)];
end

legend(plots,M);

title(sprintf('DET for Adaboost (Testing Set), Max Feature = %d',nFeatures));
xlabel('False Alarm Rate (%)');
ylabel('Miss Rate (%)');
%hold on;
