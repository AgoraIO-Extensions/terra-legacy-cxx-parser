module.exports = {
  projects: [
    {
      displayName: 'test',
      coverageDirectory: 'test',
      preset: 'ts-jest',
      testEnvironment: 'node',
      testRegex: './*/__tests__/.*\\.(test|spec)\\.[jt]sx?$',
    },
    {
      displayName: 'integration',
      coverageDirectory: 'integration',
      preset: 'ts-jest',
      testEnvironment: 'node',
      testRegex: './*/__integration_test__/.*\\.integration\\.test\\.[jt]sx?$',
    },
    
  ],
};
