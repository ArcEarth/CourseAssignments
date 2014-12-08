function im = rescaleImageVectors(im)
  % im = rescaleImageVectors(im)
  % Input: N x K matrix im, with K images each with N pixels.
  % Normalize each column such that
  %  im(:,k) = (im(:,k) - a * ones) * scl
  % with the resulting image having sum(im(:,k)) = 0 and
  % sum(im(:,k).^2) = N ( or zero).
    
  n = size(im,1);
  s = sum(im,1)/n;
  im = im - repmat(s,n,1);
  scl = sqrt(sum(im.^2, 1)/n);
  idx = scl > 0;
  if any(idx)
    im(:,idx) = im(:,idx) ./ repmat(scl(idx), n, 1);
  end
  return;