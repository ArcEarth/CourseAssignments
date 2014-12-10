function [hx,hy,tau] = calculateDET(featList, nFeatures, X,y)
    sumAlpha = sum([featList.alpha]);
    resp = evalBoosted(featList, nFeatures, X);
    tau = (-1:0.005:1.01)' * sumAlpha;
    hy = 1 - sum(bsxfun(@gt,resp(y>0),tau),2) ./ sum(y>0);
    hx = 1 - sum(bsxfun(@lt,resp(y==0),tau),2) ./ sum(y==0);
    hy(isnan(hy)) = 0;
    hx(isnan(hx)) = 0;
return;