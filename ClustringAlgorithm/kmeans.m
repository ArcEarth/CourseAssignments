function [ Cat ] = kmeans( A,k )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
N = length(A);
M = size(A,2);

while true
    %Initialize Centrid with random value
    Centriod = zeros(k,M);
    range = floor(N/k);
    for i = 1:k
        c = (i-1)*range + randi(range,1);
        Centriod(i,:) = A(c,:);
    end

    D = Inf*ones(N,k);
    flag = true;
    Cat = ones(N,1,'uint32');

    count = 0;
    while flag && count < 500
        flag = false;
        for i = 1:N
            for j = 1:k
                T = Centriod(j,:)-A(i,:);
                D(i,j) = T*T';
            end
            [~,idx] = min(D(i,:));
            if (idx ~= Cat(i))
                Cat(i) = idx;
                flag = true;
            end
        end

        for j = 1:k
            Cj = Cat == j;
            Aj = A(Cj,:);
            assert(sum(Cj)~=0);
            Centriod(j,:) = sum(Aj)/sum(Cj);
        end
        count = count + 1;
    end
    if (count < 500) 
        return
    end
end

end

