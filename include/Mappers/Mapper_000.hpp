#ifndef MAPPER_000_HPP
#define MAPPER_000_HPP

#include "../Mapper.hpp"
#include "../Typedefs.hpp"

class Mapper_000 : public Mapper
{
public:
    Mapper_000(uint8_t prgBanks, uint8_t chrBanks);
    ~Mapper_000();

    virtual bool cpuMapRead(Address address, uint32_t &mappedAddress) override;
    virtual bool cpuMapWrite(Address address, uint32_t &mappedAddress) override;
    virtual bool ppuMapRead(Address address, uint32_t &mappedAddress) override;
    virtual bool ppuMapWrite(Address address, uint32_t &mappedAddress) override;
};

#endif