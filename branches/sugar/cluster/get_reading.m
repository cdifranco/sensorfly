function rangeDist = get_reading(realDist, sigMeanDist)

rangeDist = interp1(sigMeanDist(:, 1), sigMeanDist(: ,2), realDist, 'cubic');