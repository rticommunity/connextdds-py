pipeline{
    agent none
    options {
      disableConcurrentBuilds()
    }
    stages{
        stage("Build Wheels"){
            parallel {
                stage("Build Linux Wheels"){
                    agent {
                        label "docker"
                    }
                    steps{
                        checkout BbS(
                            branches: [[name: 'develop']],
                            credentialsId: 'bitbucket-build-credentials',
                            extensions: [
                            [$class: 'RelativeTargetDirectory', relativeTargetDir: 'connextdds']
                            ],
                            id: '30271d0b-165e-4c8e-93ac-e979df5f8f7c',
                            mirrorName: '',
                            projectName: 'connext',
                            repositoryName: 'connextdds',
                            serverId: 'c7d6946a-118c-49a4-b2e7-46393b01b0fb', sshCredentialsId: ''
                        )
                        sh "resources/jenkins/buildwheel.sh"
                    }
                    post{
                        cleanup {
                            cleanWs()
                        }
                        success {
                            archiveArtifacts(
                                artifacts: 'wheelhouse/*.whl',
                                caseSensitive: false,
                                fingerprint: true,
                                followSymlinks: false,
                                onlyIfSuccessful: true
                            )
                            script{
                                if (env.BRANCH_NAME == 'develop') {
                                    docker.image('apihackers/twine').inside() {
                                        withCredentials([
                                            usernamePassword(
                                                    credentialsId: 'artifactory-credentials',
                                                    usernameVariable: 'ARTIFACTORY_USER',
                                                    passwordVariable: 'ARTIFACTORY_PASS'
                                            )
                                        ]) {
                                            sh "twine upload --repository-url https://repo.rti.com/artifactory/api/pypi/inttools-pypi-dev-local/ -u ${ARTIFACTORY_USER} -p ${ARTIFACTORY_PASS} wheelhouse/*"
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                stage("Build Darwin Wheels"){
                    agent {
                        label "darwin17-ci"
                    }
                    environment {
                        CI = 'true'
                    }
                    steps{
                        checkout BbS(
                            branches: [[name: '*/develop']],
                            credentialsId: 'bitbucket-build-credentials',
                            extensions: [
                                [$class: 'CloneOption', honorRefspec: true, noTags: true, reference: '', shallow: true, timeout: 20],
                                [$class: 'RelativeTargetDirectory', relativeTargetDir: 'connextdds']
                            ],
                            id: '6a11cf7f-10be-4997-b48f-e9eebb9ddc2f',
                            mirrorName: '',
                            projectName: 'connext',
                            repositoryName: 'connextdds',
                            serverId: 'c7d6946a-118c-49a4-b2e7-46393b01b0fb', sshCredentialsId: ''
                        )
                        sh "resources/jenkins/buildwheel-darwin.sh"
                    }
                    post{
                        cleanup {
                            cleanWs()
                        }
                        success {
                            archiveArtifacts(
                                artifacts: 'wheelhouse/*.whl',
                                caseSensitive: false,
                                fingerprint: true,
                                followSymlinks: false,
                                onlyIfSuccessful: true
                            )
                            script{
                                if (env.BRANCH_NAME == 'develop') {
                                    withPythonEnv("python3") {
                                        sh "pip install twine"
                                        withCredentials([
                                            usernamePassword(
                                                    credentialsId: 'artifactory-credentials',
                                                    usernameVariable: 'ARTIFACTORY_USER',
                                                    passwordVariable: 'ARTIFACTORY_PASS'
                                            )
                                        ]) {
                                            sh "twine upload --repository-url https://repo.rti.com/artifactory/api/pypi/inttools-pypi-dev-local/ -u ${ARTIFACTORY_USER} -p ${ARTIFACTORY_PASS} wheelhouse/*"
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                stage("Build Windows x64 wheels"){
                    agent {
                        docker {
                            image 'repo.rti.com:443/docker-local/doozer-win64-msvc14:ltsc2019'
                            label 'docker-win'
                            alwaysPull true
                        }
                    }
                    steps{
                        checkout BbS(
                            branches: [[name: 'develop']],
                            credentialsId: 'bitbucket-build-credentials',
                            extensions: [
                            [$class: 'RelativeTargetDirectory', relativeTargetDir: 'connextdds']
                            ],
                            id: '30271d0b-165e-4c8e-93ac-e979df5f8f7c',
                            mirrorName: '',
                            projectName: 'connext',
                            repositoryName: 'connextdds',
                            serverId: 'c7d6946a-118c-49a4-b2e7-46393b01b0fb', sshCredentialsId: ''
                        )
                        bat "resources/jenkins/buildwheel-win64.bat"
                    }
                    post{
                        cleanup {
                            cleanWs(disableDeferredWipeout: true)
                        }
                        success {
                            archiveArtifacts(
                                artifacts: 'wheelhouse/*.whl',
                                caseSensitive: false,
                                fingerprint: true,
                                followSymlinks: false,
                                onlyIfSuccessful: true
                            )
                            script{
                                if (env.BRANCH_NAME == 'develop') {
                                    withPythonEnv("python") {
                                        bat "pip install twine"
                                        withCredentials([
                                            usernamePassword(
                                                    credentialsId: 'artifactory-credentials',
                                                    usernameVariable: 'ARTIFACTORY_USER',
                                                    passwordVariable: 'ARTIFACTORY_PASS'
                                            )
                                        ]) {
                                            bat "twine upload --repository-url https://repo.rti.com/artifactory/api/pypi/inttools-pypi-dev-local/ -u ${ARTIFACTORY_USER} -p ${ARTIFACTORY_PASS} wheelhouse/*"
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                stage("Build Windows x32 wheels"){
                    agent {
                        docker {
                            image 'repo.rti.com:443/docker-local/doozer-win64-msvc14:ltsc2019'
                            label 'docker-win'
                            alwaysPull true
                        }
                    }
                    steps{
                        checkout BbS(
                            branches: [[name: 'develop']],
                            credentialsId: 'bitbucket-build-credentials',
                            extensions: [
                            [$class: 'RelativeTargetDirectory', relativeTargetDir: 'connextdds']
                            ],
                            id: '30271d0b-165e-4c8e-93ac-e979df5f8f7c',
                            mirrorName: '',
                            projectName: 'connext',
                            repositoryName: 'connextdds',
                            serverId: 'c7d6946a-118c-49a4-b2e7-46393b01b0fb', sshCredentialsId: ''
                        )
                        bat "resources/jenkins/buildwheel-win32.bat"
                    }
                    post{
                        cleanup {
                            cleanWs(disableDeferredWipeout: true)
                        }
                        success {
                            archiveArtifacts(
                                artifacts: 'wheelhouse/*.whl',
                                caseSensitive: false,
                                fingerprint: true,
                                followSymlinks: false,
                                onlyIfSuccessful: true
                            )
                            script{
                                if (env.BRANCH_NAME == 'develop') {
                                    withPythonEnv("python") {
                                        bat "pip install twine"
                                        withCredentials([
                                            usernamePassword(
                                                    credentialsId: 'artifactory-credentials',
                                                    usernameVariable: 'ARTIFACTORY_USER',
                                                    passwordVariable: 'ARTIFACTORY_PASS'
                                            )
                                        ]) {
                                            bat "twine upload --repository-url https://repo.rti.com/artifactory/api/pypi/inttools-pypi-dev-local/ -u ${ARTIFACTORY_USER} -p ${ARTIFACTORY_PASS} wheelhouse/*"
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

    }
}