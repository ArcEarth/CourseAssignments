function [Bet] = AddBetweenness( i,j,P,A )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
    N = max(size(A));
    Bet = sparse(N,N);
    if (i == j) 
        return;
    end
    if (i > j)
        t = i;
        i = j;
        j = t;
    end
%     fprintf('(%d->%d->%d)\n',i,P(i,j),j);
    if (A(i,j)) 
        Bet(i,j) = 1;
    else
        k = P(i,j);
        Bet = Bet + AddBetweenness(i,k,P,A);
        Bet = Bet + AddBetweenness(k,j,P,A);
    end

end

