function [ Bet ] = betweenness( A )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
N=length(A);
Bet = zeros(N,N);

idx = A~=0;
Bet(idx) = 1;

% disp (D);

for k=1:N
    [~,P] = dijkstra(A,k);
    %disp(P);
    for i=k+1:N
        j = i;
        while (P(j)>0)
            Bet(j,P(j)) = Bet(j,P(j)) + 1;
            j = P(j);
        end
    end
end

Bet = sparse(Bet);

% fprintf('D(%d,%d) = %d, Path = %d\n',2,115,D(2,115),P(2,115));
% fprintf('D(%d,%d) = %d, Path = %d\n',1,2,D(1,2),P(1,2));
% disp(D);
% disp(P);

% for i=1:N
%     for j=i+1:N
%         if (~A(i,j) && D(i,j)<Inf)
%             Bet = Bet + AddBetweenness(i,j,P,A);
%         end
%     end
% end

end

