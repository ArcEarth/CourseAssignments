function [catalog] = girvannewman(A,k)
    A = A|A';
%     disp(A);
    com = scomponents(A);
    cc = max(com);
    while cc < k
%         fprintf('current components : %d\n',cc);
        Bet = betweenness(A);
%         disp(Bet);
        val = max(max(Bet));
        idx = Bet==val;
        idx = idx|idx';
%         disp(idx);
        A(idx) = false;
        com = scomponents(A);
        cc = max(com);
    end
    catalog = uint32(com);
    return
end