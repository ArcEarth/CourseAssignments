function [ Cat ] = rcut( A,K )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
N = length(A);
W= A|A';
D = W*ones(N,1);
D = diag(D);
L = D-W;
[V,~] = eig(L);
V = V(:,1:K);
% disp(V);
% plot(1:N,V);
Cat = kmeans(V,K);
end

