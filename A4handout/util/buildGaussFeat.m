function [f] = buildGaussFeat(p)
  % [f] = buildGaussFeat(p)
  % Build gaussian derivative filters with parameters set by the struct p.
  % Input:
  %  p.sz  - [ny, nx] image size for 2D feature
  %  p.sigma  - standard dev of gaussian.  This should be
  %             such that 4*sigma+1 <= min(nx, ny)
  %  p.deriv  - 1 or 2 for 1st or 2nd order directional derivative
  %             0 difference of gaussians, surround sigma is p.sigma,
  %               center sigma is p.sigma/2.  (p.theta not used).
  %  p.theta  - orientation of directional derivative in radians
  %  p.center  - pixel [x0, y0] for center of filter, with 
  %                2sigma <= x0 <= nx - 2sigma
  %                2sigma <= y0 <= ny - 2sigma
  %              That is, the center position should not be closer
  %              than 2*sigma from any side of the feature 
  % Output: 
  %  f - a ny x nx matrix corresponding to gaussian derivative filter.
    
  f = zeros(p.sz);
  sigma = p.sigma;
  
  % Build 1D gaussian filter masks, and 1st and 2nd derivs
  gFiltRad = round(3.5*sigma);
  gFiltSize = 2 * gFiltRad + 1;
  x = [1:gFiltSize] - round((gFiltSize+1)/2);
  sigmaSqr = sigma*sigma;
  gFilt = exp(- x .* x / (2.0*sigmaSqr));
  gFilt = gFilt/ sum(gFilt(:));
  gxFilt = - (x / sigmaSqr) .* gFilt;
  gxxFilt = ((x / sigmaSqr).^2 - 1.0/sigmaSqr) .* gFilt;
  
  % Steer the requested directional derivative.
  ct = cos(p.theta);
  st = sin(p.theta);
  switch p.deriv
   case 0  % DOG filter, surround filter is gFilt.
    % Build sigma/2 Gaussian;
    cFilt = exp(- x .* x / (8.0*sigmaSqr));
    cFilt = cFilt/sum(cFilt);
    gTheta = cFilt(:)*cFilt + gFilt(:) * gFilt;
   case 1
    gx = gFilt(:) * gxFilt;
    gy = gxFilt(:) * gFilt;
    gTheta = ct * gx + st*gy;
   case 2
    gxx = gFilt(:) * gxxFilt;
    gyy = gxxFilt(:) * gFilt;
    gxy = gxFilt(:) * gxFilt;
    gTheta = (ct * ct) * gxx + (2 * ct *st) * gxy + (st * st) * gyy;
   otherwise
    fprintf('Unknown derivative type: %d\n', p.deriv);
    return;
  end
  
  % Normalize filter
  gTheta = gTheta/sqrt(sum(gTheta(:).^2));
  
  % Build a 2D gaussian with the same sigma
  g0 = gFilt(:) * gFilt;
  
  % Crop and pad the feature and the 2D gaussian
  % into the requested image size.
  x0 = round(p.center(1));
  y0 = round(p.center(2));
  padTop = y0 - (gFiltRad+1);
  padLeft = x0 - (gFiltRad+1);
  if padTop < 0
    gTheta = gTheta((-padTop):end, :);
    g0 = g0((-padTop):end, :);
    padTop = 0;
  end
  if padLeft < 0
    gTheta = gTheta(:, (-padLeft):end);
    g0 = g0(:, (-padLeft):end);
    padLeft = 0;
  end
  
  % Do the copy
  nxCopy = min(size(gTheta,2), size(f,2)-padLeft);
  nyCopy = min(size(gTheta,1), size(f,1)-padTop);
  f(padTop+(1:nyCopy), padLeft+(1:nxCopy)) = gTheta(1:nyCopy, 1:nxCopy);
  g = zeros(size(f));
  g(padTop+(1:nyCopy), padLeft+(1:nxCopy)) = g0(1:nyCopy, 1:nxCopy);
  
  % Make the mean of the feature equal to zero
  mf = sum(sum(f));
  mg = sum(sum(g));
  f = f - (mf/mg) * g;
  
  return;
  
