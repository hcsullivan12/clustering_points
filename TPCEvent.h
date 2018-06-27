#ifndef TPCEVENT_HH
#define TPCEVENT_HH

struct TPCEvent {
  
  public:
    unsigned run;
    unsigned subrun;
    unsigned event;
    bool clustering;
    std::vector< std::vector<double> > data;
    std::vector< std::vector< std::vector<double> > > cluster_data; 
};

#endif
