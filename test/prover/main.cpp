#include <stdio.h>
#include "starks.hpp"
#include "proof2zkinStark.hpp"
#include "zkevmSteps.hpp"

int main()
{

    Config config;
    config.runFileGenBatchProof = true;
    config.zkevmConstPols = "./constants.bin";
    config.mapConstPolsFile = false;
    config.zkevmConstantsTree = "./mycircuit.consttree";
    config.zkevmStarkInfo = "./starkinfo.json";

    StarkInfo starkInfo(config, config.zkevmStarkInfo);


    uint64_t polBits = starkInfo.starkStruct.steps[starkInfo.starkStruct.steps.size() - 1].nBits;
    FRIProof fproof((1 << polBits), FIELD_EXTENSION, starkInfo.starkStruct.steps.size(), starkInfo.evMap.size(), starkInfo.nPublics);

    void *pCommit = copyFile("/data/commits.bin", starkInfo.nCm1 * sizeof(Goldilocks::Element) * (1 << starkInfo.starkStruct.nBits));
    void *pAddress = (void *)calloc(starkInfo.mapTotalN + (starkInfo.mapSectionsN.section[eSection::cm1_n] * (1 << starkInfo.starkStruct.nBits) * FIELD_EXTENSION ), sizeof(uint64_t));

        Starks starks(config, {config.zkevmConstPols, config.mapConstPolsFile, config.zkevmConstantsTree, config.zkevmStarkInfo},pAddress);


    std::memcpy(pAddress, pCommit, starkInfo.nCm1 * sizeof(Goldilocks::Element) * (1 << starkInfo.starkStruct.nBits));

    Goldilocks::Element publicInputs[0];

    json publicStarkJson;
/*
    for (int i = 0; i < 47; i++)
    {
        publicStarkJson[i] = Goldilocks::toString(publicInputs[i]);
    }
*/
    ZkevmSteps zkevmSteps;
    starks.genProof(fproof, &publicInputs[0], &zkevmSteps);

    nlohmann::ordered_json jProof = fproof.proofs.proof2json();
    nlohmann::json zkin = proof2zkinStark(jProof);
    // Generate publics
    jProof["publics"] = publicStarkJson;
    zkin["publics"] = publicStarkJson;

    json2file(zkin, "zkin.json");
    json2file(jProof, "jProof.json");

    return 0;
}
