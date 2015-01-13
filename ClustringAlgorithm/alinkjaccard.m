function [ Cat ] = alinkjaccard( A, k )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

N = length(A);
I = zeros(N,N);
D = eye(N);
A = A|A';
Cat = (1:N)';
I = double(A);
I = I*I;
% disp(I);
B = double(~A);
D = B*B;
D = N*ones(N) - D;
% disp(D);
D = I ./ D;
% for j = 1:N
%     for i = j+1:N
%         for p = 1:N
%             if (A(i,p) || A(j,p)) 
%                 D(i,j) = D(i,j) + 1;
%                 if (A(i,p) && A(j,p))
%                     I(i,j) = I(i,j) + 1;
%                 end
%             end
%         end
%         if (D(i,j)~=0)
%             D(i,j) = I(i,j) / D(i,j);
%         end
% %         disp(i);
%         D(j,i) = D(i,j);
%     end
% end

% fprintf('Initialize finished.\n');
% disp(D);

CS = zeros(N,N);
R = (1:N)';

for p = N:-1:k+1
    for i = 1:p
        for j = i+1:p
            C1 = Cat==R(i);
            C2 = Cat==R(j);
            ret = C1'*D*C2;
            ret = ret / (sum(C1)*sum(C2));
            CS(i,j) = ret;
        end
    end
%     fprintf('Round %d.\n',N-p+1);
%     disp(ClusterSimilarity);
    [val,cols] = max(CS);
    [val,y] = max(val);
    x = cols(y);
    assert(y>x);

    Cat(Cat==R(y)) = R(x);
    R(y:p-1) = R(y+1:p);
    CS = CS(1:p-1,1:p-1);
end

Map = zeros(N);
for i=1:k
    Map(R(i)) = i;
end
for i=1:N
    Cat(i) = Map(Cat(i));
end

end

