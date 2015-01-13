function [ Cat ] = ncut( A,K )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
N = length(A);
W= A|A';
D = W*ones(N,1);
InvD = 1 ./ sqrt(D);
D = diag(D);
L = D-W;
InvD = diag(InvD);
L = InvD * L * InvD;
[V,~] = eig(L);
V = V(:,1:K);
% disp(V);
Cat = kmeans(V,K);
end

