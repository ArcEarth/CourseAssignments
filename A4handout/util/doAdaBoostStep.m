function wghts = doAdaBoostStep(wghts, alpha, H, y)
  
  ea = exp(alpha);
  idx = (H ~= y);
  wghts(idx) = wghts(idx) * ea;
 
  return;