#ifndef FILEREADER_HH
#define FILEREADER_HH

#include "TPCEvent.h"

class FileReader {

  public:
    FileReader(std::string filename);
    void ReadFile();  
    void PrintData();
    void PrintClusterData();
    std::vector<TPCEvent> GetEvents() { return events; };

  private:
    std::string filename; 
    bool isClustering = false;
    std::vector<TPCEvent> events;
};

#endif

FileReader::FileReader(std::string filename)
{
  this->filename = filename;
}

void FileReader::ReadFile()
{
  std::ifstream f;
  f.open(filename.c_str());

  if (f.is_open()) { 
    std::string line = "";
    std::istringstream iss(line);
    std::vector<std::string> words(( std::istream_iterator<std::string>(iss) ), std::istream_iterator<std::string>());
    unsigned run_temp;
    bool found_run = false;

    // Get first line
    std::getline(f, line);
    iss   = std::istringstream(line);
    words = std::vector<std::string>(( std::istream_iterator<std::string>(iss) ), std::istream_iterator<std::string>());
    if (words.at(0) == "Run") found_run = true;

    while (found_run) {
      found_run = false;
      // Create a new tpc event
      TPCEvent tpc_event;
      // Run
      tpc_event.run = std::stoi(words.at(1));
      // Subrun
      std::getline(f,line);
      iss = std::istringstream(line);
      words = std::vector<std::string>(( std::istream_iterator<std::string>(iss) ), std::istream_iterator<std::string>());
      tpc_event.subrun = std::stoi(words.at(1));
      // Event
      std::getline(f,line);
      iss = std::istringstream(line);
      words = std::vector<std::string>(( std::istream_iterator<std::string>(iss) ), std::istream_iterator<std::string>());
      tpc_event.event = std::stoi(words.at(1));

      // Start reading the data for this event
      std::vector<std::vector<double>> this_cluster;
      while (std::getline(f,line)) {
        iss = std::istringstream(line);
        words = std::vector<std::string>(( std::istream_iterator<std::string>(iss) ), std::istream_iterator<std::string>());
  
        // Check if this is next run
        if (words.at(0) == "Run") {
          // Finish up last event
          tpc_event.cluster_data.push_back(this_cluster);
          this_cluster.clear();
          found_run = true;
          break;
        }

        // Check if this is a cluster
        if (words.at(0) == "Cluster") {
          isClustering = true;
          tpc_event.clustering = true;
          if (this_cluster.size() > 0) {
            tpc_event.cluster_data.push_back(this_cluster);
            this_cluster.clear();
          }
          continue;
        }

        // Store the data
        std::vector<double> points;
        points.push_back(std::stod(words.at(0))); points.push_back(std::stod(words.at(1))); 
        points.push_back(std::stod(words.at(2))); points.push_back(std::stod(words.at(3)));
        if (isClustering) this_cluster.push_back(points);
        tpc_event.data.push_back(points);
      } // while getline

      // Check for end of file
      if (f.eof()) {
        tpc_event.cluster_data.push_back(this_cluster);
        this_cluster.clear();
      }

      // Make sure there are space points in this event
      if (tpc_event.data.size() == 0) continue;
      events.push_back(tpc_event);
      
    } // end if found_run
  } // end if file open
}

void FileReader::PrintData()
{
  for (const auto &e : events) {
    std::cout << "Run "    << e.run    << "\n"
              << "Subrun " << e.subrun << "\n"
              << "Event "  << e.event  << "\n";
    for (const auto &p : e.data) {
      std::cout << p.at(0) << " " << p.at(1) << " " << p.at(2) << std::endl;
    }
  }
}

void FileReader::PrintClusterData()
{
  for (const auto &e : events) {
    std::cout << "Run "    << e.run    << "\n"
              << "Subrun " << e.subrun << "\n"
              << "Event "  << e.event  << "\n";
    unsigned c = 0;
    for (const auto &cl : e.cluster_data) {
      std::cout << "Cluster " << c << "\n";
      for (const auto &p : cl) {
        std::cout << p.at(0) << " " << p.at(1) << " " << p.at(2) << std::endl;
      }
      c++;
    }
  }
}
