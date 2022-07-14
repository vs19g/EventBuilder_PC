#ifndef EVB_WORKSPACE_H
#define EVB_WORKSPACE_H

namespace EventBuilder {

    class EVBWorkspace
    {
    public:
        EVBWorkspace(const std::string& workspace);
        ~EVBWorkspace();

        inline const bool IsValid() const { return m_isValid; }

        inline std::string GetBinaryDir() const { return m_binaryDir; }
        inline std::string GetTempDir() const { return m_tempDir; }
        inline std::string GetSortedDir() const { return m_sortedDir; }
        inline std::string GetBuiltDir() const { return m_builtDir; }
        inline std::string GetMergedDir() const { return m_mergedDir; }

        std::vector<std::string> GetBinaryRunRange(int runMin, int runMax);
        std::vector<std::string> GetBuiltRunRange(int runMin, int runMax);
        
        bool UnpackBinaryRunToTemp(int run); //Currently Linux/MacOS only. Windows support to come.
        std::vector<std::string> GetTempFiles();
        bool ClearTempDirectory();
        //Maybe offload to another class? Idk. Feel like EVBWorkspace shouldn't know about ROOT
        bool MergeBuiltFiles(const std::string& outputname, int runMin, int runMax);

    private:
        void Init();
        std::string GetBinaryRun(int run);
        std::string GetBuiltRun(int run);
        bool m_isValid;

        std::string m_workspace;

        std::string m_binaryDir;
        std::string m_tempDir;
        std::string m_sortedDir;
        std::string m_builtDir;
        std::string m_mergedDir;
    };
}

#endif