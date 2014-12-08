function bestFeat = fishFeature(fSz, sigmaSet, X, y, wghts, ...
                                greedyErr, debug) 

  if ~exist('greedyErr', 'var')
    greedyErr = 0;
  end

  if ~exist('debug', 'var')
    debug = 0;
  end
  
  % Find the best feature, it has to be better than this....
  bestFeat.err = 1;
  
  for sigma = sigmaSet(:)'
    pFeat.sz = fSz;
    pFeat.sigma = sigma;
    % Select grid such that every sample is no closer than
    % 2*sigma from the image borders, and spaced by dx.
    dx = max(round(pFeat.sigma),1);

    % Choose nx st (1 + dx * (nx-1) + 4* sigma) <= pFeat.sz(2)
    % similarly for ny.
    nx = floor((pFeat.sz(2) - 4*pFeat.sigma - 1)/dx)+1;
    ny = floor((pFeat.sz(1) - 4*pFeat.sigma - 1)/dx)+1;

    % Grid starts at sx,sy, with shifts of size dx in both x and y.
    sx = round((pFeat.sz(2) - (1 + dx * (nx - 1)))/2);
    sy = round((pFeat.sz(1) - (1 + dx * (ny - 1)))/2);

    for deriv = 0:2
      pFeat.deriv = deriv;
      
      maxQuartTheta = 0;
      if deriv > 0
        maxQuartTheta = 3;
      end
      for quartTheta = 0:maxQuartTheta
        pFeat.theta = quartTheta * (pi/4);
        
        for i0 = 0:(nx-1)
          x0 = sx+i0*dx;
          for j0 = 0:(ny-1)
            y0 = sy+j0*dx;
            
            pFeat.center = [x0 y0];
            f = buildGaussFeat(pFeat);

            if debug
              figure(1); clf; 
              showIm(f);
              pause(0.1);
            end

            for useAbs = 0:1
              pFeat.abs = (useAbs == 1);
              
              [pFeat.err, pFeat.thres, pFeat.parity, H] = ...
                  trainStump(f(:), pFeat.abs, X, y, wghts, debug);
              
              if pFeat.err < greedyErr
                bestFeat = pFeat;
                return;
              end
              
              if bestFeat.err > pFeat.err
                bestFeat = pFeat;
              end
            end  % abs
          end % j0
        end % i0
      end % quartTheta
    end % deriv
  end % sigma    
  return;