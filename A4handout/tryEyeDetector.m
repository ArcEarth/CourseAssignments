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

figure(1); clf;
showIm(im);


