function [ Cat ] = modularity( A,K )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
N = length(A);
m = 2*sum(sum(A));
W= A|A';
D = W*ones(N,1);
D = D*D'/(2*m);
B = W-D;
[V,~] = eig(B);
V = V(:,N-K+1:N);
% disp(V);
% plot(1:N,V);
Cat = kmeans(V,K);
end

