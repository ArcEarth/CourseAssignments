%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  File: tryEyeDetector.m
%  Matlab script file
%  Date: Oct, 08
%

% Dependencies, Toolboxes:
%      iseToolbox/
% Author: YOU
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%% Check Path and Constants  %%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
clear;
%close all;

if ~exist('buildGaussFeat','file')
  addpath('./util');
end

%  Check path.  If these aren't on your path, then you will need
%  to add these toolboxes to your path.  Use the startup m-file provided
%  at the beginning of the course.
which pgmRead          % should be in iseToolbox\pyrTools\MEX\

im = pgmRead('tryEye.pgm');

load('adaFit');
load('testSet');

figure(1); clf;
showIm(im);

flag = true;

F = sizeIm(1)*sizeIm(2);

center = floor(0.5 * sizeIm);
ny = -floor((sizeIm(1)-1) * 0.5);
nx = -floor((sizeIm(2)-1) * 0.5);
py = ceil((sizeIm(1)-1)*0.5);
px = ceil((sizeIm(2)-1) * 0.5);
assert(px-nx+1==sizeIm(2));

while true
    figure(1);
    [x,y] = ginput(2);
    if (size(x,1) < 2)
        break;
    end;
    x = floor(x);
    x = sort(x);
    y = floor(y);
    y = sort(y);
    X = zeros(F,(x(2)-x(1)+1)*(y(2)-y(1)+1));
    for i = max(y(1),1-ny) : min(y(2),size(im,1)-py)
        for j = max(x(1),1-nx) : min(x(2),size(im,2)-px)
            idx = (i-y(1))*(x(2)-x(1)+1)+j-x(1)+1;
            Xc = reshape(im(ny+i:i+py,nx+j:j+px),[F,1]);
%             figure(5);
%             showIm(reshape(Xc,sizeIm));
            X(:,idx) = Xc;
        end
    end
    X = rescaleImageVectors(X);
    resp = evalBoosted(featList, nFeatures, X);
    idx = find(resp>19);
    ey = floor(idx / (x(2)-x(1)+1)) + y(1);
    ex = mod(idx,x(2)-x(1)+1) + x(1);
    figure(1);
    hold on;
    plot(ex,ey,'o');
end

