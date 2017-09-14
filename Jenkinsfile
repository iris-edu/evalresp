pipeline
{
    agent none
    stages
    {
        stage('fetch')
        {
            agent any
            steps
            {
                checkout  changelog: true, poll: true, scm: [$class: 'GitSCM', branches: [[name: "origin/master"]], doGenerateSubmoduleConfigurations: true, extensions: [[$class: 'CloneOption', timeout: 15, shallow: false]], submoduleCfg: [], userRemoteConfigs: [[name: 'origin', url: 'git@github.com:iris-edu/evalresp.git', credentialsId: 'be834f90-9480-48ef-a09c-d47b25318e29']]]
                stash 'source'
            }
        }
        step {stage('build & test')
        {
            parallel centos6:
            {
                node('centos64') {
                    sh "echo centos 6.4"
                    unstash 'source'
                }
            },
            centos7:
            {
                node('centos7')
                {
                    sh "echo centos 7"
                    unstash 'source'
                }
            },
            windwos:
            {
                node('windows10')
                {
                    bat "echo windows 10"
                    unstash 'source'
                }
            }
        }}
        /*
        stage('build & test - centos 6.4')
        {
            agent {node{label 'centos64'}}
            steps
            {
                unstash 'source'
                sh 'sudo yum --enablerepo=epel install -y autoconf libtool libxml2-devel check-devel  python-pip doxygen'
                sh 'sudo pip install --upgrade pip robotframework virtualenv virtualenvwrapper'
                sh 'tests/jenkins/build-evalresp.sh'
                sh 'tests/jenkins/run-c-tests.sh'
                sh './tests/jenkins/build-extended-robot-tests.sh 2017 1'
                sh './tests/jenkins/build-extended-robot-tests.sh 2010 365'
                sh './tests/jenkins/run-robot-tests.sh'
                stash name: 'store_C64'
            }
            post {
                always {
                    step([$class: 'RobotPublisher',
                      disableArchiveOutput: false,
                      logFileName: 'tests/robot/log.html',
                      otherFiles: '',
                      outputFileName: 'tests/robot/output.xml',
                      outputPath: '.',
                      passThreshold: 100,
                      reportFileName: 'tests/robot/report.html',
                      unstableThreshold: 0])
                }
                failure {
                    archiveArtifacts artifacts: **'/tests/robot/run/*', fingerprint: true
                }
            }
        }
        stage('build & test - centos 7')
        {
            agent {node{label 'centos7'}}
            steps
            {
                unstash 'source'
                sh 'sudo yum --enablerepo=epel install -y autoconf libtool libxml2-devel check-devel doxygen'
                sh 'sudo pip install --upgrade pip robotframework virtualenv virtualenvwrapper'
                sh 'tests/jenkins/build-evalresp.sh'
                //sh 'mkdir m4'
                //sh 'autoreconf -ivf'
                //sh './configure --prefix $(pwd)/install --enable-check --enable-debug --enable-shared --enable-lib-mode'
                //sh 'make'
                sh './tests/jenkins/run-c-tests.sh'
                //sh 'sudo make install'
                sh './tests/jenkins/build-extended-robot-tests.sh 2017 1'
                sh './tests/jenkins/build-extended-robot-tests.sh 2010 365'
                //sh './tests/jenkins/build-extended-robot-tests.sh 2010 365 *.G.RER.00.BHZ'
                sh './tests/jenkins/run-robot-tests.sh'
                stash 'store_check_7'
            }
            post {
                always {
                    step([$class: 'RobotPublisher',
                      disableArchiveOutput: false,
                      logFileName: 'tests/robot/log.html',
                      otherFiles: '',
                      outputFileName: 'tests/robot/output.xml',
                      outputPath: '.',
                      passThreshold: 100,
                      reportFileName: 'tests/robot/report.html',
                      unstableThreshold: 0])
                }
                failure {
                    archiveArtifacts artifacts: **'/tests/robot/run/*', fingerprint: true
                }
            }
        }
        stage('build & test - windows 10')
        {
            agent { node { label 'windows10'}}
            steps
            {
                unstash 'source'
                bat 'tests\\jenkins\\build-evalresp.bat'
                bat 'tests\\jenkins\\build-extended-robot-tests.bat 2017 1'
                bat 'tests\\jenkins\\build-extended-robot-tests.bat 2010 365'
                bat 'tests\\jenkins\\run-robot-tests.bat'
            }
            post {
                always {
                    step([$class: 'RobotPublisher',
                      disableArchiveOutput: false,
                      logFileName: 'tests/robot/log.html',
                      otherFiles: '',
                      outputFileName: 'tests/robot/output.xml',
                      outputPath: '.',
                      passThreshold: 100,
                      reportFileName: 'tests/robot/report.html',
                      unstableThreshold: 0])
                }
                failure {
                    archiveArtifacts artifacts: **'/tests/robot/run/*', fingerprint: true
                }
            }
        }
        */
        /*stage('distbuild')
        {
            agent { node {label 'centos7'}}
            steps
            {
                sh 'sudo yum --enablerepo=epel install -y autoconf libtool libxml2-devel doxygen'
                unstash 'store_check_7'
                echo 'this is tbd'
            }
        }*/
    }
}